# Self Balancing Robot

Two wheeled balancing robots are based on inverted pendulum configuration which rely upon dynamic balancing systems for balancing and maneuvering. These robot bases provide exceptional robustness and capability due to their smaller size and power requirements. Outcome of research in this field had led to the birth of robots such as Segway, Murata boy etc. Such robots find their applications in surveillance & transportation purpose. This project is based on development of a self-balanced two-wheeled robot which has a configuration similar to a bicycle. In particular, the focus is on the electro-mechanical mechanisms & control algorithms required to enable the robot to perceive and act in real time for a dynamically changing world. While these techniques are applicable to many robot applications, the construction of sensors, filters and actuator system is a learning experience.
Two wheeled balancing robot is a classic engineering problem based on inverted pendulum and is much like trying to balance a broom on the tip of your finger.

MPU is needed to provide the angle position of the inverted pendulum or robot base and input into the microcontroller, which the program in itself is a balancing algorithm. The microcontroller will then provide a type of feedback signal through PWM control to turn the motor clockwise or anticlockwise, thus balancing the robot. The code is written in C software and compiled for the Tiva Launchpad tm4c123g microcontroller, which is interfaced with the sensors and motors. The main goal of the microcontroller is to fuse the wheel encoder, gyroscope and accelerometer sensors to estimate the attitude of the platform and then to use this information to drive the reaction wheel in the direction to maintain an upright and balanced position. The basic idea for a two-wheeled dynamically balancing robot is pretty simple: move the actuator in a direction to counter the direction of fall. In practice this requires two feedback sensors: a tilt or angle sensor to measure the tilt of the robot with respect to gravity, an accelerometer to calibrate the gyro thus minimizing the drift.


### HARDWARE REQUIREMENTS
1. DC motors with high resolution encoder
2. Motor driver L298N
3. MPU9250
4. 12v Battery
5. Tiva Launchpad TM4C123G microcontroller

### SOFTWARE REQUIREMENTS
1. Code Composer Studio
2. Energia for real time graph plotting

## Circuit Diagram
![alt text](https://github.com/avg598/Self-Balancing-Robot/blob/master/documentation/circuit_diagram.png "Refer to report for more information")
