/*
Created:	9/3/2015
Author:		Anton
*/

#ifndef _TIMER_h
#define _TIMER_h

#if defined(ARDUINO) && ARDUINO >= 100
#include "arduino.h"
#else
#include "WProgram.h"
#endif

class Timer
{
private:
	unsigned long last_time;
	unsigned long interval;

public:
	Timer(unsigned long interval); // Interval is in milliseconds.
	void resetTimer(); // Resets timer to 0
	bool isItTimeYet(); // Returns true if time > interval. False otherwise.
};

#endif

