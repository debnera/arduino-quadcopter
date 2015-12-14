/*
Created:	9/2/2015 11:27:21 AM
Author:	Anton
*/

#include "commandlist.h"
CommandList::CommandList()
{
	length = 0;
	commands = 0;
	functions = 0;
}

CommandList::~CommandList()
{
	if (commands != 0) delete[] commands;
	if (functions != 0) delete[] functions;
}

void CommandList::addCommand(String command, FunctionPointer function)
{
	length++;
	String *new_commands = new String[length];
	FunctionPointer *new_functions = new FunctionPointer[length];
	for (int i = 0; i < length - 1; i++)
	{
		new_commands[i] = commands[i];
		new_functions[i] = functions[i];
	}
	if (commands != 0) delete[] commands;
	if (functions != 0) delete[] functions;
	new_commands[length - 1] = command;
	new_functions[length - 1] = function;
	commands = new_commands;
	functions = new_functions;
}

void CommandList::remove(int index)
{
	if (index >= 0 && index < length)
	{
		length--;
		String *new_commands = new String[length];
		FunctionPointer *new_functions = new FunctionPointer[length];
		int a = 0;
		for (int i = 0; i < length - 1; i++)
		{
			if (i != index)
			{
				new_commands[a] = commands[i];
				new_functions[a] = functions[i];
				a++;
			}
		}
		if (commands != 0) delete[] commands;
		if (functions != 0) delete[] functions;
		commands = new_commands;
		functions = new_functions;
	}
}

int CommandList::getIndex(String command)
{
	for (int i = 0; i < length; i++)
	{
		if (commands[i].equalsIgnoreCase(command))
		{
			return i;
		}
	}
	return -1;
}

void CommandList::callFunction(int index)
{
	if (index >= 0 && index < length)
	{
		functions[index]();
	}
}