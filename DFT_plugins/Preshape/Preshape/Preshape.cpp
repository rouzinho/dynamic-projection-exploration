/*======================================================================================================================

    Copyright 2011, 2012, 2013, 2014, 2015, 2016, 2017 Institut fuer Neuroinformatik, Ruhr-Universitaet Bochum, Germany

    This file is part of cedar.

    cedar is free software: you can redistribute it and/or modify it under
    the terms of the GNU Lesser General Public License as published by the
    Free Software Foundation, either version 3 of the License, or (at your
    option) any later version.

    cedar is distributed in the hope that it will be useful, but WITHOUT ANY
    WARRANTY; without even the implied warranty of MERCHANTABILITY or
    FITNESS FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public
    License for more details.

    You should have received a copy of the GNU Lesser General Public License
    along with cedar. If not, see <http://www.gnu.org/licenses/>.

========================================================================================================================

    Institute:   Ruhr-Universitaet Bochum
                 Institut fuer Neuroinformatik

    File:        Preshape.cpp

    Maintainer:  Stephan Zibner
    Email:       stephan.zibner@ini.ruhr-uni-bochum.de
    Date:        2011 11 08

    Description:

    Credits:

======================================================================================================================*/

// CEDAR INCLUDES
#include "Preshape.h"
#include "cedar/auxiliaries/math/transferFunctions/AbsSigmoid.h"
#include "cedar/auxiliaries/MatData.h"
#include "cedar/processing/DeclarationRegistry.h"
#include "cedar/processing/ElementDeclaration.h"
#include "cedar/auxiliaries/assert.h"
#include "cedar/auxiliaries/math/sigmoids.h"
#include "cedar/auxiliaries/math/tools.h"
#include "cedar/units/Time.h"
#include "cedar/units/prefixes.h"
#include <iostream>

// SYSTEM INCLUDES
#include <vector>

//----------------------------------------------------------------------------------------------------------------------
// register the class
//----------------------------------------------------------------------------------------------------------------------
/*namespace
{
  bool declare()
  {
    using cedar::proc::ElementDeclarationPtr;
    using cedar::proc::ElementDeclarationTemplate;

    ElementDeclarationPtr declaration
    (
      new cedar::proc::ElementDeclarationTemplate<Preshape>("Utilities")
    );
    declaration->setIconPath(":/steps/preshape.svg");
    declaration->setDescription
    (
      "A dynamical system that slowly accumulates activation at locations of strongly active input. The activation "
      "decays slowly over time."
    );

    declaration->declare();

    return true;
  }

  bool declared = declare();
}*/

//----------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------
Preshape::Preshape()
:
mActivation(new cedar::aux::MatData(cv::Mat::zeros(50,50,CV_32F))),
mOutput(new cedar::aux::MatData(cv::Mat::zeros(50,50,CV_32F))),
mTopic_in(new cedar::aux::StringParameter(this, "Topic input", "")),
mTopic_out(new cedar::aux::StringParameter(this, "Topic output", "")),
_mDimensionality
(
  new cedar::aux::UIntParameter(this, "dimensionality", 2, cedar::aux::UIntParameter::LimitType::positiveZero(4))
),
mChoice
(
  new cedar::aux::UIntParameter(this, "choice", 0, cedar::aux::UIntParameter::LimitType::positiveZero(4))
),
_mSizes(new cedar::aux::UIntVectorParameter(this, "sizes", 2, 50, 1, 5000)),
_mTimeScaleBuildUp
(
  new cedar::aux::DoubleParameter(this, "time scale build up", 100.0, cedar::aux::DoubleParameter::LimitType::positive())
),
_mTimeScaleDecay
(
  new cedar::aux::DoubleParameter(this, "time scale decay", 1000.0, cedar::aux::DoubleParameter::LimitType::positive())
),
_mSigmoid
(
  new Preshape::SigmoidParameter
  (
    this,
    "sigmoid",
    cedar::aux::math::SigmoidPtr(new cedar::aux::math::AbsSigmoid(0.5, 1000.0))
  )
),
it_(nh_)
{
  _mSizes->makeDefault();
  QObject::connect(_mSizes.get(), SIGNAL(valueChanged()), this, SLOT(dimensionSizeChanged()));
  QObject::connect(_mDimensionality.get(), SIGNAL(valueChanged()), this, SLOT(dimensionalityChanged()));
  QObject::connect(mTopic_in.get(), SIGNAL(valueChanged()), this, SLOT(reNameSubscriber()));
  QObject::connect(mTopic_out.get(), SIGNAL(valueChanged()), this, SLOT(reNamePublisher()));
  auto activation_slot = this->declareOutput("activation", mActivation);
  activation_slot->setSerializable(true);
  mt_state = cv::Mat::zeros(100,100,CV_32F);
  this->declareInput("input", true);
  this->declareInput("inhibition",false);
  this->declareInput("peak detector", false);
  connected = false;
  choice = 0;
  new_mt = false;
  publish_mt = false;
  // now check the dimensionality and sizes of all matrices
  this->updateMatrices();
  this->connect(this->mChoice.get(), SIGNAL(valueChanged()), this, SLOT(reChoice()));
  this->registerFunction("reset memory", boost::bind(&Preshape::resetMemory, this));
}
//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------

void Preshape::eulerStep(const cedar::unit::Time& time)
{
  if(new_mt)
  {
    this->mActivation->setData(mt_state);
    new_mt = false;
  }
  cv::Mat& preshape = this->mActivation->getData();
  cedar::aux::ConstDataPtr input = this->getInput("input");
  //const cv::Mat& input_mat = input->getData<cv::Mat>(); //comment
  cedar::aux::ConstDataPtr inhibition = this->getInput("inhibition");

  const cv::Mat& sum = input->getData<cv::Mat>().clone();
  
  const double& tau_build_up = this->_mTimeScaleBuildUp->getValue();
  const double& tau_decay = this->_mTimeScaleDecay->getValue();

  //cv::Mat sigmoided_input = tanActivation(input_mat);

  if(auto inh_detector = boost::dynamic_pointer_cast<cedar::aux::ConstMatData>(this->getInput("inhibition")))
  {
    bool res = MeaningfulInh(inhibition->getData<cv::Mat>().clone());
    if(res)
    {
      sum += inhibition->getData<cv::Mat>().clone();     //uncomment here
    }
  }
  cv::Mat sigmoided_input = this->_mSigmoid->getValue()->compute(sum);//sum
  double peak = 1.0;
  if (auto peak_detector = boost::dynamic_pointer_cast<cedar::aux::ConstMatData>(this->getInput("peak detector")))
  {
    peak = cedar::aux::math::getMatrixEntry<double>(peak_detector->getData(), 0, 0);
    connected = true;
  }
  else
  {
    connected = false;
  }

  // one possible preshape dynamic
  if(choice == 0)
  {
    preshape +=
    (
      time / cedar::unit::Time(tau_build_up * cedar::unit::milli * cedar::unit::seconds)
        * (-1.0 * preshape + sum).mul(sigmoided_input)
      + time / cedar::unit::Time(tau_decay * cedar::unit::milli * cedar::unit::seconds)
        * (-1.0 * preshape.mul((sigmoided_input)))
    ) * peak;
  }
  if(choice == 1)
  {
    if((connected && peak > 0.1) || !connected)
    {
      preshape +=
      (
        time / cedar::unit::Time(tau_build_up * cedar::unit::milli * cedar::unit::seconds)
          * (-1.0 * preshape + sum).mul(sigmoided_input)
        + time / cedar::unit::Time(tau_decay * cedar::unit::milli * cedar::unit::seconds)
          * (-1.0 * preshape.mul((1 - sigmoided_input)))
      ) * peak;
    }
  }
  if(publish_mt)
  {
    //std::cout<<"publishing\n";
    sendMt(preshape);
  }
  ros::spinOnce();
}

bool Preshape::MeaningfulInh(const cv::Mat& sum)
{
  bool res = false;
  for(int i = 0; i < sum.rows; i++)
  {
    for(int j = 0; j < sum.cols; j++)
    {
      float tmp = sum.at<float>(i,j);
      if(tmp < -0.5 || tmp > 0.05)
      {
        res = true;
      }
    }
  }

  return res;
}

void Preshape::mtCallback(const sensor_msgs::ImageConstPtr& msg)
{
   new_mt = true;
   cv_bridge::CvImagePtr cv_ptr;
   try
   {
      cv_ptr = cv_bridge::toCvCopy(msg, sensor_msgs::image_encodings::TYPE_32FC1);
      mt_state = cv::Mat::zeros(cv_ptr->image.rows,cv_ptr->image.cols,CV_32F);
      for(int i = 0; i < cv_ptr->image.rows; i++)
      {
          for(int j = 0; j < cv_ptr->image.cols; j++)
          {
            float tmp = cv_ptr->image.at<float>(i,j);
            mt_state.at<float>(i,j) = tmp;
          }
      }
   }
   catch (cv_bridge::Exception& e)
   {
      ROS_ERROR("cv_bridge exception: %s", e.what());
      return;
   }
}

cedar::proc::DataSlot::VALIDITY Preshape::determineInputValidity
                                                         (
                                                           cedar::proc::ConstDataSlotPtr slot,
                                                           cedar::aux::ConstDataPtr data
                                                         ) const
{
  if (slot->getRole() == cedar::proc::DataRole::INPUT && slot->getName() == "input")
  {
    if (cedar::aux::ConstMatDataPtr input = boost::dynamic_pointer_cast<const cedar::aux::MatData>(data))
    {
      if (!this->isMatrixCompatibleInput(input->getData()))
      {
        return cedar::proc::DataSlot::VALIDITY_ERROR;
      }
      else
      {
        return cedar::proc::DataSlot::VALIDITY_VALID;
      }
    }
    return cedar::proc::DataSlot::VALIDITY_ERROR;
  }
  else if (slot->getRole() == cedar::proc::DataRole::INPUT && slot->getName() == "inhibition")
  {
    if (cedar::aux::ConstMatDataPtr input = boost::dynamic_pointer_cast<const cedar::aux::MatData>(data))
    {
      if (!this->isMatrixCompatibleInput(input->getData()))
      {
        return cedar::proc::DataSlot::VALIDITY_ERROR;
      }
      else
      {
        return cedar::proc::DataSlot::VALIDITY_VALID;
      }
    }
    return cedar::proc::DataSlot::VALIDITY_ERROR;
  }
  else if (slot->getRole() == cedar::proc::DataRole::INPUT && slot->getName() == "peak detector")
  {
    if (cedar::aux::ConstMatDataPtr input = boost::dynamic_pointer_cast<const cedar::aux::MatData>(data))
    {
      if (cedar::aux::math::getDimensionalityOf(input->getData()) != 0)
      {
        return cedar::proc::DataSlot::VALIDITY_ERROR;
      }
      else
      {
        return cedar::proc::DataSlot::VALIDITY_VALID;
      }
    }
  }
  return cedar::proc::DataSlot::VALIDITY_ERROR;
}

cv::Mat Preshape::tanActivation(const cv::Mat& inpGain)
{
    cv::Size s = inpGain.size();
    cv::Mat output = cv::Mat::zeros(s.height, s.width, CV_32F);
    for(int i = 0; i < s.height; i++)
    {
        for(int j = 0; j < s.width;j++)
        {
            output.at<float>(i,j) = (exp(inpGain.at<float>(i,j)) - exp(-inpGain.at<float>(i,j))) / (exp(inpGain.at<float>(i,j)) + exp(-inpGain.at<float>(i,j)));
        }
    }
    return output;
}

bool Preshape::isMatrixCompatibleInput(const cv::Mat& matrix) const
{
  if (matrix.type() != CV_32F)
  {
    return false;
  }

  unsigned int matrix_dim = cedar::aux::math::getDimensionalityOf(matrix);
  return this->_mDimensionality->getValue() == matrix_dim
           && cedar::aux::math::matrixSizesEqual(matrix, this->mActivation->getData());
}

void Preshape::dimensionalityChanged()
{
  this->_mSizes->resize(_mDimensionality->getValue(), _mSizes->getDefaultValue());
  this->updateMatrices();
}

void Preshape::dimensionSizeChanged()
{
  this->updateMatrices();
}

void Preshape::resetMemory()
{
  mActivation->getData() = 0.0;
}

void Preshape::updateMatrices()
{
  int dimensionality = static_cast<int>(_mDimensionality->getValue());

  std::vector<int> sizes(dimensionality);
  for (int dim = 0; dim < dimensionality; ++dim)
  {
    sizes[dim] = _mSizes->at(dim);
  }
  // check if matrices become too large
  double max_size = 1.0;
  for (int dim = 0; dim < dimensionality; ++dim)
  {
    max_size *= sizes[dim];
    if (max_size > std::numeric_limits<unsigned int>::max()/100.0) // heuristics
    {
      CEDAR_THROW(cedar::aux::RangeException, "cannot handle matrices of this size");
      return;
    }
  }
  this->lockAll();
  if (dimensionality == 0)
  {
    this->mActivation->getData() = cv::Mat(1, 1, CV_32F, cv::Scalar(0));
  }
  else if (dimensionality == 1)
  {
    this->mActivation->getData() = cv::Mat(sizes[0], 1, CV_32F, cv::Scalar(0));
  }
  else
  {
    this->mActivation->getData() = cv::Mat(dimensionality,&sizes.at(0), CV_32F, cv::Scalar(0));
  }
  this->unlockAll();
  this->revalidateInputSlot("input");
  this->emitOutputPropertiesChangedSignal("activation");
}

void Preshape::reChoice()
{
  choice = this->mChoice->getValue();
}

void Preshape::reNameSubscriber()
{
  try
  {
    name_port_in = this->mTopic_in->getValue();
    const std::string tname = name_port_in;
    const std::string comp = "";
    if(tname != comp)
    {
      sub_mt = it_.subscribe(tname, 1, &Preshape::mtCallback, this);
    }
  }
  catch (ros::Exception &e)
  {
    ROS_ERROR("Error occured: %s ", e.what());
  }
   
}

void Preshape::reNamePublisher()
{
  name_port_out = this->mTopic_out->getValue();
  const std::string tname_out = name_port_out;
  const std::string comp = "";
  if(tname_out != comp)
  {
    pub_mt = it_.advertise(tname_out, 1);
    publish_mt = true;
  }
}

void Preshape::sendMt(cv::Mat& mt)
{
  cv::Mat field = cv::Mat(mt.rows,mt.cols, CV_32FC1);
  mt.convertTo(field,CV_32FC1);
  sensor_msgs::ImagePtr msg = cv_bridge::CvImage(std_msgs::Header(), "32FC1", field).toImageMsg();
  pub_mt.publish(msg);
}
