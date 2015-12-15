/*
Created:	8/24/2015
Author:	Anton

This class handles reading from and writing to an external device
through the software serial interface (e.g. with bluetooth).
*/

#include "serial_communicator.h"


SerialCommunicator::SerialCommunicator(int pin_rx, int pin_tx, long baudrate)
{
	soft_serial = new SoftwareSerial(pin_rx, pin_tx, false); // false for non-inverted logic
	soft_serial->begin(baudrate);
}

/*SerialCommunicator::~SerialCommunicator()
{
soft_serial->end();
delete soft_serial;
}*/

void SerialCommunicator::write(String message)
{
	// Sends the message to the connected device over software serial.
	// SoftwareSerial.print is non-blocking meaning that the message is actually
	// send in the background without pausing this program.
	//
	// NOTE:	If the writebuffer(64 bytes) is filled, print becomes blocking
	//			and pauses until it can send the message.
	if (message.endsWith(String('\n')) == false)
	{
		// Ensure the message ends in newline, otherwise it will be parsed
		// incorrectly by the receiving device.
		message += '\n';
	}
	soft_serial->print(message);
}

void SerialCommunicator::readFromSerial()
{
	// Reads all data that is waiting in the serial buffer.
	// Because the serial connection is relatively slow, we cannot expect
	// to receive the whole message at once. Therefore we will gather the 
	// received data in read_buffer, and parse it from there.
	while (soft_serial->available() && read_buffer.length() < kMaxBufferSize)
	{
		read_buffer += soft_serial->read();
	}
	if (read_buffer.length() >= kMaxBufferSize)
	{
		// We are receiving data faster than we can process.
		write("WARNING: Read buffer overflow");
	}
}

String SerialCommunicator::getCompletedCommand()
{
	// Checks if read_buffer has any complete commands which end in newline(\n)
	// and returns the first such command. Also removes that command from read_buffer.
	String command;
	int end = read_buffer.indexOf('\n'); // returns -1 if not found
	if (end >= 0)
	{
		for (int i = 0; i < end + 1; i++)
		{
			command += read_buffer[i];
		}
		read_buffer.remove(0, end + 1);
	}
	command.trim();
	return command;
}
