# SPH
A real time, GPU-based, fluid simulation using smooth particle hydrodynamics done as a solo project for my Computer Animation (COMP 6311) class in Concordia University, Montreal. The project is written in C++ and the rendering is done using OpenGl, all SPH computations occur in the GPU through the use of compute shaders, nearest neighbour computations are also done through the use of multiple compute shaders. The simulation is able to render more than 100,000 particles in real time because of the parallel nature of the GPU. 

Contents:

- [Images](#images)


- [Videos](#videos)


- [Setup Guide](#setup-guide)


- [Functionality](#functionality)


- [External Libraries](#external-libraries-used)


- [References](#references-and-attributions)




### Images 

![](images/FluidSim.png)
![](images/collision.png)


### Videos 

https://user-images.githubusercontent.com/63361048/215293472-1cc3614a-0f61-4051-bbd6-c7e501cec304.mp4


https://user-images.githubusercontent.com/63361048/215293476-6ee23b1d-3c10-4e40-82f6-2c35470af15c.mp4







### Setup Guide

#### Requirements:

 - Windows OS (10+)

 - Minimum 1536 work groups supported by the GPU 
    - This program makes use of compute shaders through OpenGL. At the momment, the number of work groups in the shader is set to 1536.
      If your GPU supports less than 1536 work groups then you will have to go into the .comp files included in the project and change the 
      work group size in local_size_x. 
      
#### Using the program:
  Simply download the SPH folder and launch the executable. (NOTE: It is important that you launch the executable in the folder provided otherwise it   will be unable to locate the shader files)
      
      

### Functionality

#### Parameter Tuning
Through the ImGui UI the user can manipulate the parameters of the SPH simulation.
These parameters include the dynamic viscocity coefficient, the number of particles and more.
Warning, at the extremes the parameters become unstable and may result in unexpected behavior.

#### Bounding Box Tuning
The scale of the bounding box can be manipulated in real time, the outline can also be toggled on or off

#### Object Tuning
An object can also be added to the scene, this object can collide with the smoothed particles and multiple
object configurations can be explored


### External Libraries Used
- GLFW
- Glad
- ImGui


### References and Attributions

#### LearnOpenGL
The camera, shader, and compute shader class used in this project are taken from the OpenGl tutorial series LearnOpenGL.
The classes have been slightly modified to provide functionality specific to my use case.
https://learnopengl.com/Getting-started/Camera
https://learnopengl.com/Getting-started/Shaders
https://learnopengl.com/Guest-Articles/2022/Compute-Shaders/Introduction

LearnOpenGl code is available for use under the following liscence:

https://creativecommons.org/licenses/by-nc/4.0/legalcode

#### OpenGlTutorial
The following learn-opengl tutorial was used to guide my implementation of the billboarded spheres used for rendering.
http://www.opengl-tutorial.org/intermediate-tutorials/billboards-particles/billboards/

#### Fluid Engine Development
The following book by doyub-kim was used to learn about smooth particle hydrodynamics and the smoothing kernels used in this project
are taken directly from this source.
Kim, D. (2017). Fluid Engine Development (1st ed.). A K Peters/CRC Press. https://doi.org/10.1201/b22137
