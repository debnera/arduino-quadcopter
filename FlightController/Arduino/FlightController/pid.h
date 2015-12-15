/*
Created:	8/29/2015
Author:		Anton
*/
#ifndef _PID_h
#define _PID_h

class PID
{
public:
	float k_p;
	float k_i;
	float k_d;
	float imax;
	PID(float k_p = 0, float k_i = 0, float k_d = 0, float imax = 0)
	{
		this->k_p = k_p;
		this->k_i = k_i;
		this->k_d = k_d;
		this->imax = imax;
	};
};

#endif
