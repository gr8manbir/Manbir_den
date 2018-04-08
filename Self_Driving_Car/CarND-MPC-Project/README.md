# CarND-Controls-MPC
Self-Driving Car Engineer Nanodegree Program

---

## Model Predictive Controller

The goal of this project is to drive a car around a simulated race track in a Udacity simulator.
An MPC model is an improvement over the PID model wherein it allows actuator controls values to be calculated from 
a state in the future. So the goal of this project is:

1) Calculate the path i.e. fit a polynomial for the given way points.

2) Predict the future state of the vehicle using a vehicle motion model.

3) From the future state get the actuator controls using a model predictive controller. 


## implementation

This involves the following steps:

1) The simulator provides a set of way points (x,y). These have to be transformed from global co-ordinates to vehicle co-ordinates.
This means that (x,y) = (0,0) is the position of the car.

2) From the transformed way points, fit a 3rd order polynomial function. This will give a set of co-efficients.

3) The initial cross track error and error in orientation ( not steering angle ) can be got as below:
	a. cte = initial 0 value of coeffs
	b. epsi = tan inverse of coeffs[1]
	
4) Next step is to predict the vehicle state after latency ( actual advantage of MPC ). In this step
we calculate the entire 6 vector values of the motion model after 0.1 sec( 100 ms ). This 100ms comes from
the thread sleep value which introduces the latency. The six values after 0.1 sec are calculated as follows:
	a. X after 100ms  = velocity(v) * latency time(dt)
	b. Y after 100ms = 0.0
	c. psi ( i.e. vehicle orientation ) = -v * steer *dt/Lf
	d. vel = vel + throttle*dt
	e. cte = cte + v*sin(epsi) *dt
	f. epsi = epsi + psi ( from c. )
	
5) Now we have next vehicle state and coeffs of the polynomial on which the car must remain. So using an Model predictive
controller on (next vehicle state, coeffs) we can get the following:
	a. Next steering angle
	b. Next speed
	c. (x,y) co-rordinates for the vehicle movement. This is the green line shown in final video.
	
6) We also display a yellow line just to see that the polynomial fit is being generated properly. For this we 
do the following:
	a. Use 25 point in 2.5 step increments for x co-ords
	b. Evaluate the y value for the above 25 x values from the coeffs.
	c. Plot a line with the x and y values. This is the yellow line.
	
##Model and MPC

This describes how the vehicle motion model is considered and how the model predictive controller works
1. The vehicle motion model considers 4 parameters:
	a. x state
	b. y state
	c. v(elocity) state
	d. psi( vehicle orientation )
	
2. There are two possible actuator inputs
	a. steering angle (delta) constrained to [-25, 25] degrees
	b. acceleration( a ) constrained to [-1, 1] 

3. The two errors to be reduced are cte and epsi( error in vehicle orientation )

The model works as follows:

1. Define a cost for cte, epsi, vel, steering angle, acceleration, change in steering angle and change in acceleration.
   This is done as follows:
    cost = pow( curr_state - refer_state, 2)
    Examples of reference state are speed=100, cte=0.

2. Define the weightage of each cost. For example cte=0 is more important than velocity=100.

3. Next we extrapolate these constraints for entire 10 ( i.e. N ) steps using motion model equations.

4. The MPC.solve() does the following:

	a. Set the lower and upper bound constraints for all state parameters and actuator controls.
	b. From the polynomial coefficients create the costs and constraints.
	c. Solve bounds, costs and constraints to obtain next steering angle, acceleration and (x,y) of 
	   the path to be followed.
	   
##Choosing N( number of steps)  and dt ( time between steps )

The values chosen for this solution are N=10 and dt=0.1. This gives the magic figure of predicting the 
vehicle's parameters for exactly next 1 second. I tried dt>0.1 and this make the vehicle slower to react.
Increasing N>10 made the processing slower and it seems my PC couldn't keep up. So N=10 and dt=0.1 seems
like a good solution.

## Dependencies

* cmake >= 3.5
 * All OSes: [click here for installation instructions](https://cmake.org/install/)
* make >= 4.1(mac, linux), 3.81(Windows)
  * Linux: make is installed by default on most Linux distros
  * Mac: [install Xcode command line tools to get make](https://developer.apple.com/xcode/features/)
  * Windows: [Click here for installation instructions](http://gnuwin32.sourceforge.net/packages/make.htm)
* gcc/g++ >= 5.4
  * Linux: gcc / g++ is installed by default on most Linux distros
  * Mac: same deal as make - [install Xcode command line tools]((https://developer.apple.com/xcode/features/)
  * Windows: recommend using [MinGW](http://www.mingw.org/)
* [uWebSockets](https://github.com/uWebSockets/uWebSockets)
  * Run either `install-mac.sh` or `install-ubuntu.sh`.
  * If you install from source, checkout to commit `e94b6e1`, i.e.
    ```
    git clone https://github.com/uWebSockets/uWebSockets
    cd uWebSockets
    git checkout e94b6e1
    ```
    Some function signatures have changed in v0.14.x. See [this PR](https://github.com/udacity/CarND-MPC-Project/pull/3) for more details.

* **Ipopt and CppAD:** Please refer to [this document](https://github.com/udacity/CarND-MPC-Project/blob/master/install_Ipopt_CppAD.md) for installation instructions.
* [Eigen](http://eigen.tuxfamily.org/index.php?title=Main_Page). This is already part of the repo so you shouldn't have to worry about it.
* Simulator. You can download these from the [releases tab](https://github.com/udacity/self-driving-car-sim/releases).
* Not a dependency but read the [DATA.md](./DATA.md) for a description of the data sent back from the simulator.


## Basic Build Instructions

1. Clone this repo.
2. Make a build directory: `mkdir build && cd build`
3. Compile: `cmake .. && make`
4. Run it: `./mpc`.

## Tips

1. It's recommended to test the MPC on basic examples to see if your implementation behaves as desired. One possible example
is the vehicle starting offset of a straight line (reference). If the MPC implementation is correct, after some number of timesteps
(not too many) it should find and track the reference line.
2. The `lake_track_waypoints.csv` file has the waypoints of the lake track. You could use this to fit polynomials and points and see of how well your model tracks curve. NOTE: This file might be not completely in sync with the simulator so your solution should NOT depend on it.
3. For visualization this C++ [matplotlib wrapper](https://github.com/lava/matplotlib-cpp) could be helpful.)
4.  Tips for setting up your environment are available [here](https://classroom.udacity.com/nanodegrees/nd013/parts/40f38239-66b6-46ec-ae68-03afd8a601c8/modules/0949fca6-b379-42af-a919-ee50aa304e6a/lessons/f758c44c-5e40-4e01-93b5-1a82aa4e044f/concepts/23d376c7-0195-4276-bdf0-e02f1f3c665d)
5. **VM Latency:** Some students have reported differences in behavior using VM's ostensibly a result of latency.  Please let us know if issues arise as a result of a VM environment.

## Editor Settings

We've purposefully kept editor configuration files out of this repo in order to
keep it as simple and environment agnostic as possible. However, we recommend
using the following settings:

* indent using spaces
* set tab width to 2 spaces (keeps the matrices in source code aligned)

## Code Style

Please (do your best to) stick to [Google's C++ style guide](https://google.github.io/styleguide/cppguide.html).

## Project Instructions and Rubric

Note: regardless of the changes you make, your project must be buildable using
cmake and make!

More information is only accessible by people who are already enrolled in Term 2
of CarND. If you are enrolled, see [the project page](https://classroom.udacity.com/nanodegrees/nd013/parts/40f38239-66b6-46ec-ae68-03afd8a601c8/modules/f1820894-8322-4bb3-81aa-b26b3c6dcbaf/lessons/b1ff3be0-c904-438e-aad3-2b5379f0e0c3/concepts/1a2255a0-e23c-44cf-8d41-39b8a3c8264a)
for instructions and the project rubric.

## Hints!

* You don't have to follow this directory structure, but if you do, your work
  will span all of the .cpp files here. Keep an eye out for TODOs.

## Call for IDE Profiles Pull Requests

Help your fellow students!

We decided to create Makefiles with cmake to keep this project as platform
agnostic as possible. Similarly, we omitted IDE profiles in order to we ensure
that students don't feel pressured to use one IDE or another.

However! I'd love to help people get up and running with their IDEs of choice.
If you've created a profile for an IDE that you think other students would
appreciate, we'd love to have you add the requisite profile files and
instructions to ide_profiles/. For example if you wanted to add a VS Code
profile, you'd add:

* /ide_profiles/vscode/.vscode
* /ide_profiles/vscode/README.md

The README should explain what the profile does, how to take advantage of it,
and how to install it.

Frankly, I've never been involved in a project with multiple IDE profiles
before. I believe the best way to handle this would be to keep them out of the
repo root to avoid clutter. My expectation is that most profiles will include
instructions to copy files to a new location to get picked up by the IDE, but
that's just a guess.

One last note here: regardless of the IDE used, every submitted project must
still be compilable with cmake and make./

## How to write a README
A well written README file can enhance your project and portfolio.  Develop your abilities to create professional README files by completing [this free course](https://www.udacity.com/course/writing-readmes--ud777).
