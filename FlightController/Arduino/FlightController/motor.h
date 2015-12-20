/*
Created:	8/29/2015
Author:		Anton
*/

#ifndef _MOTOR_h
#define _MOTOR_h

#if defined(ARDUINO) && ARDUINO >= 100
#include "arduino.h"
#else
#include "WProgram.h"
#endif

#include <Servo.h> //Basic Arduino library
class Motor
{
private:
	Servo servo;
	int cur_power;
	String name;
	int pin;

public:
	Motor(int pin, String name);
	~Motor();
	bool attach(); // Attaches the motor - it's ready to be used.
	bool detach(); // Detaches the motor making it stop.
	void setPower(int value); // Sets motor power in microseconds.
	String getName();
};


#endif
