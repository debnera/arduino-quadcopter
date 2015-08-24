/*
Created:	8/24/2015
Author:	Anton
*/
// serial_communicator.h

#ifndef _SERIAL_COMMUNICATOR_h
#define _SERIAL_COMMUNICATOR_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif

#include <SoftwareSerial\SoftwareSerial.h>

class SerialCommunicator
{
private:
	SoftwareSerial softSerial;
	String writeBuffer;
	String readBuffer;

public:
	SerialCommunicator(int baudrate);
	void addToWriteBuffer(String str);
	void addToWriteBuffer(float value) { addToWriteBuffer(String(value)); }
	void addToWriteBuffer(int value) { addToWriteBuffer(String(value)); }
	void writeNow(String str);
	void writeNow(float value) { writeNow(String(value)); }
	void writeNow(int value) { writeNow(String(value)); }
	void readFromSerial();
	unsigned int getIncomingBufferSize() { return readBuffer.length(); }
	unsigned int getWriteBufferSize() { return writeBuffer.length(); }
	String getReceivedString();

};

#endif

