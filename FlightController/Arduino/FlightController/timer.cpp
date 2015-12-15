/*
Created:	9/3/2015
Author:		Anton
*/

#include "timer.h"

Timer::Timer(unsigned long interval)
{
	this->interval = interval;
	resetTimer();
}

void Timer::resetTimer()
{
	last_time = millis();
}

bool Timer::isItTimeYet()
{
	if (millis() >= last_time + interval)
	{
		return true;
	}
	return false;
}
