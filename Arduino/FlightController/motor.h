/*
Created:	8/29/2015
Author:		Anton
*/

#ifndef _ANGLES_h
#define _ANGLES_h

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
	int min_pwm_width;
	int max_pwm_width;
	String name;
	int pin;

public:
	// min_pwm_signal and max_pwm_signal depends on which ESC is used.
	Motor(int pin, String name, int min_pwm_signal, int max_pwm_signal);
	~Motor();
	bool attach(); // Attaches the motor - it's ready to be used.
	bool detach(); // Detaches the motor making it stop.
	void setPower(int value); // Sets motor power in range [0 , getMaxMotorPower()]
	int getMaxMotorPower(); // Returns (max_pwm_width - min_pwm_width)
	String getName();
};


#endif
