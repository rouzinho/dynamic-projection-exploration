# Dynamic Exploration through latent space projection

Welcome to the repository hosting the source code to replicate the paper "Active Exploration causes developmental changes in curiosity-based learning."

The architecture is the following :

![](https://github.com/rouzinho/dynamic-projection-exploration/blob/main/media/arch_simple_all.png?raw=true)

![](https://github.com/rouzinho/dynamic-projection-exploration/blob/main/media/learning_grasping.gif?raw=true)

The DFT_plugin folder gathers necessary plugins to run in the cedar software. You can check how to compile and use a plugin on the cedar software [website](https://cedar.ini.rub.de/). We replaced the Intel D435 with the Azure Kinect for better point cloud performance.

The Architecture file groups all the parameters of the neural fields for Cedar.

To calibrate the camera with the robot, check the [website](https://docs.trossenrobotics.com/interbotix_xsarms_docs/ros1_packages/perception_pipeline_configuration.html) of trossen robotics.

The ROS folder is a collection of ROS1 packages (Noetic). You can start the Demo by running the camera:
```
roslaunch azure_kinect_ros_driver driver_test.launch
``` 
Then the robot (after calibration):
```
roslaunch interbotix_xsarm_perception xsarm_perception.launch robot_model:=px150 use_pointcloud_tuner_gui:=true use_armtag_tuner_gui:=true
``` 
Now you can start the perception module that will detect the object rotation and translation in time:
```
roslaunch startup perceptions.launch
``` 
Open the Cedar software and initialize the ROS node within it (ROSInit plugin). You can then load the architecture file containing all parameters and agency of the model.

Launch the rest of model (VAEs, MLP) and the GUI with :
```
roslaunch startup learning.launch
``` 

The user interface allows you to pause and resume the experiment whenever you want, which is practical if the object is out of reach.
In addition, the experiment automatically pause if the object disapears from the camera (after grasping for example).