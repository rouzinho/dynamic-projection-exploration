//Plugin to read an angle from a ROS Topic and display it on a 1D neural field (gaussian centered at the angle value).

// CEDAR INCLUDES
#include "RosLatentSubscriber.h"
#include <cedar/processing/ExternalData.h> // getInputSlot() returns ExternalData
#include <cedar/auxiliaries/MatData.h> // this is the class MatData, used internally in this step
#include "cedar/auxiliaries/math/functions.h"
#include <cmath>
#include <iostream>
#include <typeinfo>

using namespace std;





// SYSTEM INCLUDES

//----------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------
RosLatentSubscriber::RosLatentSubscriber()
:
cedar::proc::Step(true),
mOutput(new cedar::aux::MatData(cv::Mat::zeros(100, 100, CV_32F))),
mSigma(new cedar::aux::DoubleParameter(this,"Sigma",3.0,cedar::aux::DoubleParameter::LimitType::positiveZero())),
mTopic(new cedar::aux::StringParameter(this, "Topic name latent", "")),
mTopic_in(new cedar::aux::StringParameter(this, "Topic name input", "")),
mTopic_out(new cedar::aux::StringParameter(this, "Topic name output", "")),
it_(n)
{
this->declareOutput("LatentSpace",mOutput);
latent_space.list_latent.resize(0);
latent_space.max_x = 100;
latent_space.max_y = 100;
size_x = 100;
size_y = 100;
sigma = 3.0;
output = cv::Mat::zeros(size_y,size_x,CV_32F);
size_sample = 0;
size_prev_sample = 0;
new_state = false;
reset_field = false;
new_mt = false;
publish_mt = false;
mGaussMatrixSizes.clear();
mGaussMatrixCenters.clear();
mGaussMatrixSigmas.clear();
mGaussMatrixSizes.push_back(100);
mGaussMatrixSizes.push_back(100);
mGaussMatrixCenters.push_back(25);
mGaussMatrixCenters.push_back(25);
mGaussMatrixSigmas.push_back(3.0);
mGaussMatrixSigmas.push_back(3.0);
this->connect(this->mSigma.get(), SIGNAL(valueChanged()), this, SLOT(resetSigma()));
this->connect(this->mTopic.get(), SIGNAL(valueChanged()), this, SLOT(reNameLatent()));
this->connect(this->mTopic_in.get(), SIGNAL(valueChanged()), this, SLOT(renameInput()));
this->connect(this->mTopic_out.get(), SIGNAL(valueChanged()), this, SLOT(renameOutput()));
}
//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------
//
void RosLatentSubscriber::compute(const cedar::proc::Arguments&)
{
   mGaussMatrixSizes.clear();
   mGaussMatrixCenters.clear();
   mGaussMatrixSigmas.clear();
   if(size_sample > size_prev_sample || new_state)
   {
      output = cv::Mat::zeros(size_y,size_x,CV_32F);
      for(int i = 0; i < size_sample; i++)
      {
         int x = static_cast<int>(latent_space.list_latent[i].x);
         int y = static_cast<int>(latent_space.list_latent[i].y);
         float val = static_cast<float>(latent_space.list_latent[i].value);
         mGaussMatrixSizes.clear();
         mGaussMatrixCenters.clear();
         mGaussMatrixSigmas.clear();
         mGaussMatrixSizes.push_back(size_y);
         mGaussMatrixSizes.push_back(size_x);
         mGaussMatrixSigmas.push_back(sigma);
         mGaussMatrixSigmas.push_back(sigma);
         mGaussMatrixCenters.push_back(y);
         mGaussMatrixCenters.push_back(x);
         cv::Mat tmp = cedar::aux::math::gaussMatrix(2,mGaussMatrixSizes,val,mGaussMatrixSigmas,mGaussMatrixCenters,false);
         output += tmp.clone();
      }
   }
   else
   {
      if(size_sample == 0 && reset_field)
      {
         output = cv::Mat::zeros(size_y,size_x,CV_32F);
         mGaussMatrixSizes.push_back(100);
         mGaussMatrixSizes.push_back(100);
         mGaussMatrixSigmas.push_back(sigma);
         mGaussMatrixSigmas.push_back(sigma);
         mGaussMatrixCenters.push_back(0);
         mGaussMatrixCenters.push_back(0);
         cv::Mat tmp = cedar::aux::math::gaussMatrix(2,mGaussMatrixSizes,0.0,mGaussMatrixSigmas,mGaussMatrixCenters,false);
         output = tmp.clone();
         reset_field = false;
      }
   }
   if(new_mt)
   {
      output = mt_state.clone();
      new_mt = false;
   }
   this->mOutput->setData(output);
   size_prev_sample = size_sample;
   new_state = false;
   if(publish_mt)
   {
      sendMt(output);
   }
   ros::spinOnce();

}

void RosLatentSubscriber::Callback(const cog_learning::LatentNNDNF::ConstPtr& msg)
{
   size_x = msg->max_x;
   size_y = msg->max_y;
   size_sample = msg->list_latent.size();
   latent_space.list_latent.resize(size_sample);
   for(int i = 0; i < size_sample; i++)
   {
      cog_learning::Goal tmp;
      tmp.x = msg->list_latent[i].x;
      tmp.y = msg->list_latent[i].y;
      tmp.value = msg->list_latent[i].value;
      latent_space.list_latent[i] = tmp;
   }
   if(size_sample == 0)
   {
      size_prev_sample = 0;
      reset_field = true;
      new_state = false;
   }
   else
   {
      new_state = true;
   }
}

void RosLatentSubscriber::mtCallback(const sensor_msgs::ImageConstPtr& msg)
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

void RosLatentSubscriber::reNameLatent()
{
   std::string name_port = this->mTopic->getValue();
   const std::string tname = name_port;
   const std::string comp = "";
   if(tname != comp)
   {
      sub = n.subscribe(tname, 10, &RosLatentSubscriber::Callback,this);
   }
}

void RosLatentSubscriber::renameInput()
{
   std::string name_port = this->mTopic_in->getValue();
   const std::string tname = name_port;
   const std::string comp = "";
   if(tname != comp)
   {
      sub_mt = it_.subscribe(tname, 1, &RosLatentSubscriber::mtCallback, this);
   }
}

void RosLatentSubscriber::renameOutput()
{
   std::string name_port = this->mTopic_out->getValue();
   const std::string tname = name_port;
   const std::string comp = "";
   if(tname != comp)
   {
      pub_mt = it_.advertise(tname, 1);
      publish_mt = true;
   } 
}

void RosLatentSubscriber::reset()
{
   output = cv::Mat::zeros(size_y,size_x,CV_32F);
   size_prev_sample = 0;
   size_sample = 0;
   reset_field = true;
}

void RosLatentSubscriber::sendMt(cv::Mat& mt)
{
  cv::Mat field = cv::Mat(mt.rows,mt.cols, CV_32FC1);
  mt.convertTo(field,CV_32FC1);
  sensor_msgs::ImagePtr msg = cv_bridge::CvImage(std_msgs::Header(), "32FC1", field).toImageMsg();
  pub_mt.publish(msg);
}

void RosLatentSubscriber::resetSigma()
{
   sigma = this->mSigma->getValue();
}