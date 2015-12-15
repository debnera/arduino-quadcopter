// mpu.h

#ifndef _MPU_h
#define _MPU_h

#if defined(ARDUINO) && ARDUINO >= 100
#include "arduino.h"
#else
#include "WProgram.h"
#endif

#include "angles.h"
#include "serial_communicator.h"

#include "I2Cdev.h"
#include "MPU6050_6Axis_MotionApps20.h"
#if I2CDEV_IMPLEMENTATION == I2CDEV_ARDUINO_WIRE
#include "Wire.h"
#endif

class MPU
{
private:
	MPU6050 mpu;
	SerialCommunicator *serCom; // For sending error messages.
	uint8_t mpuIntStatus; // Interrupt signal status
	uint16_t packetSize;


public:
	MPU(SerialCommunicator *serial_communicator);
	bool init();
	Angles getAngles();
	Angles getAngularRates();
	int getBufferSize();
};

#endif

