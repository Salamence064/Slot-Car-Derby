Thomas Ducote 832008563 thomas-ducote@tamu.edu

To compile run the following commands:
`mkdir build`
`cd build`
`cmake ..`
`make -j4`
`./SlotCar ../resources`

Tasks:
 1. Create a circular track and draw it to the screen
 2. Get a particle to move around the circular track like a slot car would (using 'a' and 'd' or left and right arrow)
 3. Model the track using a spline instead and draw it to the screen
 4. Get the particle to move around the spline track like a slot car would
 5. Facilitate movement both up and down
 6. Update to use 2 particles on the track to represent the slots for the car on the track
 7. Draw a car model at the appropriate positions given the two slot particles
 8. Add a bounding box of particles with springs connecting them to the car model to allow the car model to naturally sway
 9.  Add a ground beneath the track
 10. If the car is moving too fast (i.e. its velocity is above a certain threshold on a turn), don't apply the constraint and let the car fall onto the ground
 11. Add the bouncing effects of the car falling onto the ground