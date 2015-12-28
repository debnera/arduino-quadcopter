/*
Created:	8/29/2015
Author:		Anton
*/

#include "motor.h"

Motor::Motor(int pin, String name)
{
	this->pin = pin;
	this->name = name;
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
		if (servo.attach(pin) != 0)
		{
      setPower(cur_power);
		  return true;
		}
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

void Motor::setPower(int value)
{
	// Writes given power to the motor as microseconds. Motors can always be
	// stopped with negative values.
	if (value < 0)
	{
		value = 0;
	}
	servo.writeMicroseconds(value);
	cur_power = value; // Store the value for detach/reattach.
}

String Motor::getName()
{
	return name;
}
