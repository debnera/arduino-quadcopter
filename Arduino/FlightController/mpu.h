// mpu.h

#ifndef _MPU_h
#define _MPU_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif

#include "angles.h"


class MPU
{
private:
	//MPU6050 mpu;

public:
	MPU();
	bool init();
	Angles getAngles();
	Angles getAngularRates();
	int getBufferSize();
};

#endif

