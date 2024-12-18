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

    File:        Preshape.h

    Maintainer:  Stephan Zibner
    Email:       stephan.zibner@ini.ruhr-uni-bochum.de
    Date:        2011 11 08

    Description:

    Credits:

======================================================================================================================*/

#ifndef PRESHAPE_H
#define PRESHAPE_H

// CEDAR INCLUDES
#include "cedar/dynamics/Dynamics.h"
#include "cedar/auxiliaries/math/Sigmoid.h"
#include "cedar/auxiliaries/DoubleParameter.h"
#include "cedar/auxiliaries/UIntParameter.h"
#include "cedar/auxiliaries/UIntVectorParameter.h"
#include "cedar/auxiliaries/ObjectParameterTemplate.h"
#include "ros/ros.h"
#include <image_transport/image_transport.h>
#include <sensor_msgs/image_encodings.h>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <cv_bridge/cv_bridge.h>

// FORWARD DECLARATIONS
#include "cedar/auxiliaries/MatData.fwd.h"
//#include "Preshape.fwd.h"

// SYSTEM INCLUDES


/*!@brief A step that implements Preshape dynamics.
 */
class Preshape : public cedar::dyn::Dynamics
{
  //--------------------------------------------------------------------------------------------------------------------
  // macros
  //--------------------------------------------------------------------------------------------------------------------
  Q_OBJECT

  //--------------------------------------------------------------------------------------------------------------------
  // nested types
  //--------------------------------------------------------------------------------------------------------------------
  //!@brief a parameter for sigmoid objects
  typedef cedar::aux::ObjectParameterTemplate<cedar::aux::math::TransferFunction> SigmoidParameter;
  //!@cond SKIPPED_DOCUMENTATION
  CEDAR_GENERATE_POINTER_TYPES_INTRUSIVE(SigmoidParameter);
  //!@endcond

  //--------------------------------------------------------------------------------------------------------------------
  // constructors and destructor
  //--------------------------------------------------------------------------------------------------------------------
public:
  //!@brief The standard constructor.
  Preshape();

  //!@brief Destructor

  //--------------------------------------------------------------------------------------------------------------------
  // public methods
  //--------------------------------------------------------------------------------------------------------------------
public:
  //!@brief determine if a given Data is a valid input to the field
  cedar::proc::DataSlot::VALIDITY determineInputValidity
                                  (
                                    cedar::proc::ConstDataSlotPtr,
                                    cedar::aux::ConstDataPtr
                                  ) const;

public slots:
  //!@brief handle a change in dimensionality, which leads to creating new matrices
  void dimensionalityChanged();
  //!@brief handle a change in size along dimensions, which leads to creating new matrices
  void dimensionSizeChanged();

  void resetMemory();

  void reChoice();

  void reNameSubscriber();

  void reNamePublisher();

  void mtCallback(const sensor_msgs::ImageConstPtr& msg);

  //--------------------------------------------------------------------------------------------------------------------
  // protected methods
  //--------------------------------------------------------------------------------------------------------------------
protected:
  /*!@brief compute the euler step of the Preshape dynamics
   *
   * This is the equation:
   * \f[
   *
   * \f]
   */
  void eulerStep(const cedar::unit::Time& time);
  bool MeaningfulInh(const cv::Mat& sum);
  cv::Mat tanActivation(const cv::Mat& inpGain);
  void sendMt(cv::Mat& mt);

  //--------------------------------------------------------------------------------------------------------------------
  // private methods
  //--------------------------------------------------------------------------------------------------------------------
private:
  //!@brief update the size and dimensionality of internal matrices
  void updateMatrices();

  //!@brief check if input fits to field in dimension and size
  bool isMatrixCompatibleInput(const cv::Mat& matrix) const;


  //--------------------------------------------------------------------------------------------------------------------
  // members
  //--------------------------------------------------------------------------------------------------------------------
protected:
  //!@brief this SpaceCode matrix contains the current field activity of the NeuralField
  cedar::aux::MatDataPtr mActivation;
  cedar::aux::MatDataPtr mOutput;
  int choice;
  

private:
  // none yet
  
  //--------------------------------------------------------------------------------------------------------------------
  // parameters
  //--------------------------------------------------------------------------------------------------------------------
protected:
  //!@brief the field dimensionality - may range from 1 to 16 in principle, but more like 6 or 7 in reality
  cedar::aux::StringParameterPtr mTopic_in;
  cedar::aux::StringParameterPtr mTopic_out;
  cedar::aux::UIntParameterPtr _mDimensionality;
  cedar::aux::UIntParameterPtr mChoice;
  //!@brief the field sizes in each dimension
  cedar::aux::UIntVectorParameterPtr _mSizes;
  //!@brief time scale build up
  cedar::aux::DoubleParameterPtr _mTimeScaleBuildUp;
  //!@brief time scale decay
  cedar::aux::DoubleParameterPtr _mTimeScaleDecay;

  //! Sigmoid applied to the input.
  SigmoidParameterPtr _mSigmoid;

  

private:
  // none yet
  std::string name_port_in;
  std::string name_port_out;
  ros::NodeHandle nh_;
  image_transport::ImageTransport it_;
  image_transport::Subscriber sub_mt;
  image_transport::Publisher pub_mt;
  cv::Mat mt_state;
  bool new_mt;
  bool publish_mt;
  bool connected;
  
}; // class cedar::dyn::Preshape

#endif // CEDAR_DYN_Preshape_H
