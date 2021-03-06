/*
Created:	8/26/2015
Author:		Anton
*/

#include "angles.h"
#include <math.h>
const int Angles::kMaxDecimals = 3;
const int Angles::kInvalidNumber = -999;


Angles::Angles()
{
	yaw = 0.0;
	pitch = 0.0;
	roll = 0.0;
}

Angles::Angles(float yaw, float pitch, float roll)
{
	this->yaw = yaw;
	this->pitch = pitch;
	this->roll = roll;
}

void Angles::setValues(float yaw, float pitch, float roll)
{
	this->yaw = yaw;
	this->pitch = pitch;
	this->roll = roll;
}

Angles Angles::operator+(const Angles &other_angles)
{
	float y = this->yaw + other_angles.yaw;
	float p = this->pitch + other_angles.pitch;
	float r = this->roll + other_angles.roll;
	return Angles(y, p, r);
}

Angles Angles::operator-(const Angles &other_angles)
{
	float y = this->yaw - other_angles.yaw;
	float p = this->pitch - other_angles.pitch;
	float r = this->roll - other_angles.roll;
	return Angles(y, p, r);
}

Angles Angles::operator/(float divider)
{
	float y = this->yaw / divider;
	float p = this->pitch / divider;
	float r = this->roll / divider;
	return Angles(y, p, r);
}

Angles Angles::operator*(float multiplier)
{
	float y = this->yaw * multiplier;
	float p = this->pitch * multiplier;
	float r = this->roll * multiplier;
	return Angles(y, p, r);
}

String Angles::toString()
{
	// Makes a String form the current values.
	// This String can then be send and parsed in an external device.

	String str = "y" + String(yaw, kMaxDecimals) +
		"p" + String(pitch, kMaxDecimals) +
		"r" + String(roll, kMaxDecimals);
	return str;
}

bool Angles::fromArray(char *str, int length)
{
	String string;
	for (int i = 0; i < length; i++)
	{
		string += str[i];
	}
	return fromString(string);
}

bool Angles::fromString(String str)
{
	// Attempts to parse angle values from given String.
	// Returns true if parsing was successfull, false otherwise.
	int y_ind = str.indexOf('y');
	int p_ind = str.indexOf('p');
	int r_ind = str.indexOf('r');
	// Let's make bools to avoid a monstrously large if statement.
	bool ypr_exist = (y_ind != -1 && p_ind != -1 && r_ind != -1);
	bool ypr_correct_order = (y_ind == 0 && y_ind + 1 < p_ind && p_ind + 1 < r_ind);
	bool ypr_last_number = (int(str.length()) - 1 > r_ind); // Room for a number after r?
	if (ypr_exist && ypr_correct_order && ypr_last_number)
	{
		float y_value = stringToFloat(str, y_ind+1, p_ind);
		float p_value = stringToFloat(str, p_ind+1, r_ind);
		float r_value = stringToFloat(str, r_ind+1, str.length());
		// Angles are in range -180 to 180. Anything else is treated as an error.
		if (fabs(y_value) <= 180 && fabs(p_value) <= 180 && fabs(r_value) <= 180)
		{
			yaw = y_value;
			pitch = p_value;
			roll = r_value;
			return true;
		}
	}
	return false;
}

float Angles::stringToFloat(String str, int from_ind, int to_ind)
{
	// Attempts to parse a float from string elements in range [from_ind, to_ind-1].
	// String can contain any amount of numbers and the decimals can be separated
	// either with a dot or a comma.
	// If the String contains any non-numeric characters, kInvalidNumber is returned.

	float value = 0;
	bool decimal = false;
	bool negative = false;
	int decimal_index = 1;
	for (int i = from_ind; i < to_ind; i++)
	{
		char c = str[i];
		if ('0' <= c && c <= '9')
		{
			if (decimal == false)
				value = value * 10 + c - '0';
			else
			{
				value += (c - '0') / pow(10, decimal_index);
				decimal_index++;
			}
		}
		else if ((c == '.' || c == ',') && decimal == false)
			decimal = true;
		else if (i == from_ind && c == '-') negative = true;
		else
			return float(kInvalidNumber);
	}
	if (negative) value = -value;
	return value;
}
