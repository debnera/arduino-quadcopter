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

#include <SoftwareSerial.h>

class SerialCommunicator
{
private:
	SoftwareSerial soft_serial = SoftwareSerial(soft_serial); // Silly workaround?
	String read_buffer;
	// Buffer size needs to be restricted to avoid overflowing memory.
	// Read/writebuffers should never get large anyway, otherwise the
	// information would be processed with a delay.
	const int kMaxBufferSize = 100; 

public:
	SerialCommunicator(int pin_rx, int pin_tx, long baudrate);
	void write(String message);
	void write(float value) { write(String(value)); }
	void write(int value) { write(String(value)); }
	void readFromSerial();
	unsigned int getReceivedBufferSize() { return read_buffer.length(); }
	String getCompleteString();
};

#endif

