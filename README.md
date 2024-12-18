# Dynamic Exploration through latent space projection

Welcome to the repository hosting the source code to replicate the paper "Active Exploration causes developmental changes in curiosity-based learning."

The architecture is the following :

![](https://github.com/rouzinho/dynamic-projection-exploration/blob/main/media/arch_simple_all.png?raw=true)

![](https://github.com/rouzinho/dynamic-projection-exploration/blob/main/media/demo_finale.gif?raw=true)

The DFT_plugin folder gathers necessary plugins to run in the cedar software. You can check how to compile and use a plugin on the cedar software [website](https://cedar.ini.rub.de/).

The ROS folder is a collection of ROS1 packages (Noetic). You can start the Demo by running :
```
roslaunch startup learning.launch
``` 