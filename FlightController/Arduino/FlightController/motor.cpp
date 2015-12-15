/*
Created:	8/29/2015
Author:		Anton
*/

#include "motor.h"

Motor::Motor(int pin, String name, int min_pwm_width, int max_pwm_width)
{
	this->pin = pin;
	this->name = name;
	this->min_pwm_width = min_pwm_width;
	this->max_pwm_width = max_pwm_width;
	cur_power = 0;
	servo = Servo();
}

Motor::~Motor()
{
	detach();
}

bool Motor::attach()
{
	if (!servo.attached())
	{
		// Servo.attach() returns 0 if it fails to attach.
		if (servo.attach(pin) != 0) return true;
		setPower(cur_power);
	}
	return false;
}

bool Motor::detach()
{
	if (servo.attached())
	{
		servo.detach();
		return true;
	}
	return false;
}

int Motor::getMaxMotorPower()
{
	return max_pwm_width - min_pwm_width;
}

void Motor::setPower(int value)
{
	// Sets motor pwm signal in range [min_pwm_width, max_pwm_width].
	// In case the pwm-range is set up wrong and the motor keeps spinning 
	// at min_pwm_width, the motor can be stopped with negative values.
	//
	// NOTE: value should be in range [0, getMaxMotorPower()] 

	int microseconds = min_pwm_width + value;
	if (value < 0) microseconds = 0;
	else if (microseconds > max_pwm_width) microseconds = max_pwm_width;
	cur_power = value; // Store the value for detach/reattach.
	servo.writeMicroseconds(microseconds);
}

String Motor::getName()
{
	return name;
}