/*
Created:	9/07/2015
Author:		Anton
*/

#include "mpu.h"

MPU::MPU(SerialCommunicator *serial_communicator)
{
  serCom = serial_communicator;
}

Angles MPU::getAngles()
{
	// TODO - Implement this function.
	return Angles(0, 0, 0);
}

Angles MPU::getAngularRates()
{
	// TODO - Implement this function.
	return Angles(0, 0, 0);
}

int MPU::getBufferSize()
{
	// TODO - Implement this function.
	return 0;
}