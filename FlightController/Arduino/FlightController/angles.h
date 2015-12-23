/*
Created:	8/26/2015
Author:		Anton
*/

#ifndef _ANGLES_h
#define _ANGLES_h

#if defined(ARDUINO) && ARDUINO >= 100
#include "arduino.h"
#else
#include "WProgram.h"
#endif


class Angles
{
private:
	static const int kMaxDecimals;
	static const int kInvalidNumber;

public:
	float yaw;
	float pitch;
	float roll;

	Angles();
	Angles(float yaw, float pitch, float roll);
	void setValues(float yaw, float pitch, float roll);
	String toString();
	bool fromString(String str);
	bool fromArray(char *str, int length);
	float stringToFloat(String str, int from_ind, int to_ind);
  Angles operator+(const Angles &other_angles);
  Angles operator-(const Angles &other_angles);
	Angles operator/(float divider);
	Angles operator*(float multiplier);
};


#endif
