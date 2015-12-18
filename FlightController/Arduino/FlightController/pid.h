/*
Created:	8/29/2015
Author:		Anton
*/
#ifndef _PID_h
#define _PID_h

class PID
{
public:
	float p;
	float i;
	float d;
	float imax;
	PID(float p = 0, float i = 0, float d = 0, float imax = 0)
	{
		this->p = p;
		this->i = i;
		this->d = d;
		this->imax = imax;
	};
};

#endif
