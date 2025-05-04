Thomas Ducote 832008563 thomas-ducote@tamu.edu

The .obj file for the car model is from Free3D: https://free3d.com/3d-model/ultracompact-car-v1--571349.html

For compilation you will need to have GLFW, GLEW, GLM, and EIGEN3. They should be stored in your environment variables as GLFW_DIR, GLEW_DIR, GLM_INCLUDE_DIR, and EIGEN3_INCLUDE_DIR respectively.
To compile run the following commands:
* `mkdir build`
* `cd build`
* `cmake ..`
* `make -j4`
* `./SlotCar <Resource Directory> <Track Number>`

Track Number should range from 0 to 3.

Tasks:
 1. ~~Create a circular track and draw it to the screen~~
 2. ~~Get a particle to move around the circular track like a slot car would (using 'a' and 'd' or left and right arrow)~~
 3. ~~Model the track using a spline instead and draw it to the screen~~
 4. ~~Get the particle to move around the spline track like a slot car would~~
 5. ~~Facilitate movement both up and down~~
 6. ~~Draw a car model at the appropriate positions given the two slot particles~~
 7. ~~Add a bounding box of particles with springs connecting them to the car model to allow the car model to naturally sway~~
 8. ~~Add a ground beneath the track~~
 9. ~~If the car is moving too fast (i.e. its velocity is above a certain threshold on a turn), don't apply the constraint and let the car fall onto the ground~~
 10. ~~Add the bouncing effects of the car falling onto the ground -- basically, we will be adding collision to the ground~~

ChatGPT provided me with the constraint equations for the CircularTrack. I also used GitHub Copilot to generate boilerplate code in some of the constructors and other functions. The constraint equation for the SplineTrack was created with help from Dr. Sueda.