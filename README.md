# Slot Car Derby

A slot car you can move forward and backwards with position based dynamics. Define an arbitrary track by setting control points for a spline in the init function for scene.cpp or use one of 4 pre-built tracks.

___

## Credits

The constraint equation for the SplineTrack was created with help from Dr. Sueda. ChatGPT provided me with the constraint equations for the CircularTrack and provided assistence in constructing the orthonormal vectors for the rotation matrix used to align the car to always face forward. I also used GitHub Copilot to generate boilerplate code in some of the constructors and other functions.

The .obj file for the car model is from Free3D: https://free3d.com/3d-model/ultracompact-car-v1--571349.html

___

## Compilation

For compilation you will need to have GLFW, GLEW, GLM, and EIGEN3. They should be stored in your environment variables as GLFW_DIR, GLEW_DIR, GLM_INCLUDE_DIR, and EIGEN3_INCLUDE_DIR respectively.
To compile run the following commands:
* `mkdir build`
* `cd build`
* `cmake ..`
* `make -j4`
* `./SlotCar <Resource Directory> <Track Number>`

Track Number should range from 0 to 3.

___

## Controls

To move the car forward you can press 'w' or the up arrow, and to reverse the car, you can press 's' or the down arrow. If you wish to toggle the animation, press the space bar and you can reset the scene by pressing 'r'. Finally, you can make the car free fall by pressing 'f'.

___

## Tasks
 1. Create a circular track and draw it to the screen
 2. Get a particle to move around the circular track like a slot car would (using 'a' and 'd' or left and right arrow)
 3. Model the track using a spline instead and draw it to the screen
 4. Get the particle to move around the spline track like a slot car would
 5. Facilitate movement both up and down
 6. Add a ground beneath the track
 7. If the car is moving too fast (i.e. its velocity is above a certain threshold on a turn), don't apply the constraint and let the car fall onto the ground
 8.  Add the bouncing effects of the car falling onto the ground

___

## Contributors
 * Thomas Ducote (thomas-ducote@tamu.edu)
