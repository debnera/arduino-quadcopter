// mpu.h

#ifndef _MPU_h
#define _MPU_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif

#include "angles.h"
//#include "serial_communicator.h"

#include "I2Cdev.h"
#include "MPU6050_6Axis_MotionApps20.h"
#if I2CDEV_IMPLEMENTATION == I2CDEV_ARDUINO_WIRE
	#include "Wire.h"
#endif

class MPU
{
private:
	MPU6050 mpu;
	bool dmpReady = false;  // set true if DMP init was successful
	uint8_t mpuIntStatus;   // holds actual interrupt status byte from MPU
	uint8_t devStatus;      // return status after each device operation (0 = success, !0 = error)
	uint16_t packetSize;    // expected DMP packet size (default is 42 bytes)
	uint16_t fifoCount;     // count of all bytes currently in FIFO
	uint8_t fifoBuffer[64]; // FIFO storage buffer
	
	
	//SerialCommunicator *serCom; // For sending error messages.
	


public:
	MPU(/*SerialCommunicator *serial_communicator*/);
	bool init();
	Angles getAngles();
	Angles getAngularRates();
	int getBufferSize();
	volatile bool mpuInterrupt;
};

#endif

