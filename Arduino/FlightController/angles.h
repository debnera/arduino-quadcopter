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
	static const int kMaxDecimals = 3;
	static const int kInvalidNumber = -999;

public:
	float yaw;
	float pitch;
	float roll;

	Angles(float yaw = 0, float pitch = 0, float roll = 0);
	void setValues(float yaw, float pitch, float roll);
	String toString();
	bool fromString(String str);
	float stringToFloat(String str, int from, int to);
};


#endif

