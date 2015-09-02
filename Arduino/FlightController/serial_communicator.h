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
	const unsigned int kMaxBufferSize = 100; 

public:
	SerialCommunicator(int pin_rx, int pin_tx, long baudrate);
	~SerialCommunicator();
	void write(String message); // Sends the given string to the connected device
	void write(float value) { write(String(value)); }
	void write(int value) { write(String(value)); }
	void readFromSerial(); // Reads all available data from serialbuffer.
	unsigned int getReceivedBufferSize() { return read_buffer.length(); }
	String getCompletedCommand(); // Returns a newline-terminated command, if one has been received.
};

#endif

