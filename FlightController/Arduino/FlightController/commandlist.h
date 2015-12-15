/*
Created:	9/2/2015 11:27:21 AM
Author:	Anton
*/

#ifndef _COMMANDLIST_h
#define _COMMANDLIST_h

#if defined(ARDUINO) && ARDUINO >= 100
#include "arduino.h"
#else
#include "WProgram.h"
#endif

class CommandList
{
private:
	typedef void(*FunctionPointer)();
	String *commands;
	FunctionPointer *functions;
	int length;

public:
	CommandList();
	~CommandList();
	void addCommand(String command, FunctionPointer function); // Adds function and command to the list. 
	void remove(int index); // Removes command and function at given index.
	int getIndex(String command); // Returns the index of given command, -1 if not found.
	void callFunction(int index); // Calls function at given index.
};


#endif

