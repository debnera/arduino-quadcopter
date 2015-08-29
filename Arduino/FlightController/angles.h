// angles.h

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
	const int kMaxDecimals = 3;
	const float kInvalidNumber = -999;
public:
	float yaw;
	float pitch;
	float roll;

	Angles(float yaw = 0, float pitch = 0, float roll = 0);
	String toString();
	bool fromString(String str);
	float stringToFloat(String str, int from, int to);
};


#endif

