/*
Created:	8/29/2015
Author:		Anton
*/

#include "motor.h"

Motor::Motor(int pin)
{

	this->pin = pin;
	Serial.print("Added motor: ");
	Serial.println(this->pin);
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
			Serial.print("Attached: ");
			Serial.println(pin);
      setPower(cur_power);
		  return true;
		}
		else Serial.println("Failed to attach motor");
	}
	return false;
}

bool Motor::detach()
{
	if (servo.attached())
	{
		Serial.print("Detached: ");
		Serial.println(pin);
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
