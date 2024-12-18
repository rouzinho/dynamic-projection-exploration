//Plugin to read an angle from a ROS Topic and display it on a 1D neural field (gaussian centered at the angle value).

// CEDAR INCLUDES
#include "RosGoalPublisher.h"
#include <cedar/processing/ExternalData.h> // getInputSlot() returns ExternalData
#include <cedar/auxiliaries/MatData.h> // this is the class MatData, used internally in this step
#include "cedar/auxiliaries/math/functions.h"
#include <cmath>
#include <iostream>

using namespace std;

// SYSTEM INCLUDES

//----------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------
RosGoalPublisher::RosGoalPublisher()
:
cedar::proc::Step(true),
mInput(new cedar::aux::MatData(cv::Mat::zeros(1, 50, CV_32F))),
mTopic(new cedar::aux::StringParameter(this, "Topic field", ""))
{
this->declareInput("goal", true);
pub_once = true;
valPeak = 0;
toler = 2;
this->connect(this->mTopic.get(), SIGNAL(valueChanged()), this, SLOT(reName()));

}
//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------
//
void RosGoalPublisher::compute(const cedar::proc::Arguments&)
{
   cv::Mat& field = mInput->getData();
   cedar::aux::ConstDataPtr opX = this->getInputSlot("goal")->getData();
   field = opX->getData<cv::Mat>();
   cv::Size s = field.size();
   sizeX = s.width;
   sizeY = s.height;

   int X = 0;
   int Y = 0;

   for(int i = 0;i < sizeY;i++)
   {
      for(int j = 0;j < sizeX;j++)
      {
         if(field.at<float>(i,j) > 0.8)
         {
            if(valPeak < field.at<float>(i,j))
            {
               X = j;
               Y = i;
               valPeak = field.at<float>(i,j);
            }
         }
      }
   }
  //posX = (static_cast<double>(minX) + static_cast<double>(maxX))/2;
  //posY = (static_cast<double>(minY) + static_cast<double>(maxY))/2;
   posX = X;
   posY = Y;
   float tolX = std::abs(posX - old_posX);
   float tolY = std::abs(posY - old_posY);
   //std::cout<<tot<<"\n";

   /*if( tot > 110)
   {
      if(pub_once)
      {
         pub.publish(og);
         pub_once = false;
      }
   }
   else
   {
      pub_once = true;
   }*/
   if(X != 0 && Y != 0)
   {
      if(tolX >= toler || tolY >= toler)
      {
         og.x = X;
         og.y = Y;
         pub.publish(og);
      }
   }

  old_posX = posX;
  old_posY = posY;
  X = 0;
  Y = 0;
  valPeak = 0;

}

void RosGoalPublisher::reName()
{
   name_port = this->mTopic->getValue();
   const std::string tname = name_port;
   pub = n.advertise<cog_learning::Goal>(tname, 1);

}

void RosGoalPublisher::reCompute()
{

}


void RosGoalPublisher::reset()
{

}
