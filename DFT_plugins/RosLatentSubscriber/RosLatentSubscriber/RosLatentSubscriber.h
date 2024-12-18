/*======================================================================================================================

    Copyright 2011, 2012, 2013, 2014, 2015 Institut fuer Neuroinformatik, Ruhr-Universitaet Bochum, Germany

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

    File:        EarSubscriber.h

    Maintainer:  Tutorial Writer Person
    Email:       cedar@ini.rub.de
    Date:        2011 12 09

    Description:

    Credits:

======================================================================================================================*/

#ifndef CEDAR_MOTOR_PUBLISHER
#define CEDAR_MOTOR_PUBLISHER

// CEDAR INCLUDES
#include <cedar/processing/Step.h> // if we are going to inherit from cedar::proc::Step, we have to include the header

// FORWARD DECLARATIONS
#include <cedar/auxiliaries/MatData.fwd.h>
#include <cedar/auxiliaries/IntParameter.h>
#include <cedar/auxiliaries/DoubleParameter.h>
#include <cedar/auxiliaries/StringParameter.h>
#include <cedar/processing/sources/GaussInput.h>
#include <string>

#include "ros/ros.h"
#include <image_transport/image_transport.h>
#include <sensor_msgs/image_encodings.h>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <cv_bridge/cv_bridge.h>
#include "std_msgs/Float64.h"
#include "/home/altair/interbotix_ws/devel/include/cog_learning/LatentGoalDnf.h"
#include "/home/altair/interbotix_ws/devel/include/cog_learning/LatentNNDNF.h"
#include "/home/altair/interbotix_ws/devel/include/cog_learning/Goal.h"

// SYSTEM INCLUDES

class RosLatentSubscriber : public cedar::proc::Step
{
  Q_OBJECT
  //--------------------------------------------------------------------------------------------------------------------
  // constructors and destructor
  //--------------------------------------------------------------------------------------------------------------------
public:
  //!@brief The standard constructor.
  RosLatentSubscriber();

  //!@brief Destructor

  //--------------------------------------------------------------------------------------------------------------------
  // public methods
  //--------------------------------------------------------------------------------------------------------------------
public slots:
  // none yet
  void reNameLatent();
  void renameInput();
  void renameOutput();
  void resetSigma();
  //--------------------------------------------------------------------------------------------------------------------
  // protected methods
  //--------------------------------------------------------------------------------------------------------------------
protected:
  // none yet

  //--------------------------------------------------------------------------------------------------------------------
  // private methods
  //--------------------------------------------------------------------------------------------------------------------
private:
  // The arguments are unused here
  void compute(const cedar::proc::Arguments&);
  void Callback(const cog_learning::LatentNNDNF::ConstPtr& msg);
  void mtCallback(const sensor_msgs::ImageConstPtr& msg);
  void sendMt(cv::Mat& mt);
  //float distanceNumber(float x, float y);
  void reset();

  //--------------------------------------------------------------------------------------------------------------------
  // members
  //--------------------------------------------------------------------------------------------------------------------
protected:
  // none yet
  cedar::aux::MatDataPtr mOutput;
  cedar::aux::DoubleParameterPtr mSigma;
  cedar::aux::StringParameterPtr mTopic;
  cedar::aux::StringParameterPtr mTopic_out;
  cedar::aux::StringParameterPtr mTopic_in;
private:
  //!@brief this is the output of the computation (in this case, the summed inputs
  std::vector<unsigned int> mGaussMatrixSizes;
  std::vector<double> mGaussMatrixSigmas;
  std::vector<double> mGaussMatrixCenters;
  ros::NodeHandle n;
  image_transport::ImageTransport it_;
  image_transport::Subscriber sub_mt;
  image_transport::Publisher pub_mt;
  ros::Subscriber sub;
  cog_learning::LatentNNDNF latent_space;
  cv::Mat output;
  int size_sample;
  int size_prev_sample;
  int size_x;
  int size_y;
  bool new_state;
  bool reset_field;
  cv::Mat mt_state;
  bool new_mt;
  bool publish_mt;
  double sigma;

  //--------------------------------------------------------------------------------------------------------------------
  // parameters
  //--------------------------------------------------------------------------------------------------------------------
protected:
  // none yet

private:
  // none yet

}; // class EarSubscriber

#endif // CEDAR_TUTORIAL_SIMPLE_SUMMATION_H
