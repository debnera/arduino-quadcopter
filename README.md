# Quadcopter

This is my attempt on creating a quadcopter from scratch. The quadcopter is controller with a laptop and xbox 360 gamepad. 
Currently it uses Bluetooth for communication, but it would not be hard to use other RF protocols. Bluetooth has very limited range (10 meters).

The main code consists of two parts: FlightController for the quadcopter and GraphicalController for the laptop.

FlightController is written in C++ for Arduino and it attempts to stabilize the quadcopter.

GraphicalController is written in Python with PyQt5, PySerial (for serial communication) and PyGame (for a very simple gamepad integration). 

Status
------
Currently the code is finished and the copter is waiting to be tested. 
It is highly likely that there are still some bugs and oversights in the code, 
therefore I do not recommend that any one else attempts to fly with this code.

Parts used
----------
Quadcopter runs on Arduino Uno.

The Quadcopter uses Invensense MPU-6050 for detecting its orientation.

Communication is done with a HC-05 Bluetooth chip.

Libraries used
--------------
For the MPU-6050 I use a slightly modified version of I2Cdev (http://www.i2cdevlib.com)



Tools and stuff
---------------
This folder contains some testing sketches and tools I needed. 
For example there is a very simple script for configuring my HC-05 bluetooth chip. 

Matlab simulator
----------------
I started this project by writing a matlab simlator to get a grasp on how a quadcopter behaves.
However after I received all the parts for my quadcopter, I decided it was easier to just start implementing.
Therefore there are two similar simulators, both unfinished and probably broken. Most likely most of the code 
is just plain wrong and I have no intentions on fixing it.

