//Plugin to read an angle from a ROS Topic and display it on a 1D neural field (gaussian centered at the angle value).

// CEDAR INCLUDES
#include "Entropy.h"
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
Entropy::Entropy()
:
cedar::proc::Step(true),
mInput(new cedar::aux::MatData(cv::Mat::zeros(100, 100, CV_32F))),
mOutput(new cedar::aux::MatData(cv::Mat(1,1, CV_32F))),
mOutput_(new cedar::aux::MatData(cv::Mat(1,1, CV_32F)))
{
this->declareInput("input", true);
this->declareOutput("direct_e",mOutput);
this->declareOutput("rnd_e",mOutput_);
this->mOutput->getData().at<float>(0,0) = 0.0;
this->mOutput_->getData().at<float>(0,0) = 0.0;

}
//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------
//
void Entropy::compute(const cedar::proc::Arguments&)
{
   cv::Mat& field = mInput->getData();
   cedar::aux::ConstDataPtr opX = this->getInputSlot("input")->getData();
   field = opX->getData<cv::Mat>();
   cv::Size s = field.size();
   sizeX = s.width;
   sizeY = s.height;
   int tot = 0;

   for(int i = 0;i < sizeY;i++)
   {
      for(int j = 0;j < sizeX;j++)
      {
         if(field.at<float>(i,j) < 0.05)
         {
            tot++;
         }
      }
   }
   float size = static_cast<float>(sizeX*sizeY);
   float total = static_cast<float>(tot);
   float free = total / size;
   float used = (size - total) / size;
   //std::cout<<"free : "<<free<<" used : "<<used<<"\n";
   if(std::abs(size-total) == 0)
   {
      free = 0;
      used = 0.6;
   }
   this->mOutput->getData().at<float>(0,0) = free;
   this->mOutput_->getData().at<float>(0,0) = used;
}

void Entropy::reCompute()
{

}


void Entropy::reset()
{

}
