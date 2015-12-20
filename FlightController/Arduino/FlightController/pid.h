/*
Created:	8/29/2015
Author:		Anton
*/
#ifndef _PID_h
#define _PID_h

class PID
{
private:
	float i_sum;
	float last_error;
	float d_speed; // Current derivative; or the speed at which error changes



public:
	float p;
	float i;
	float d;
	float imax; // i term won't exceed this value
	float sample_time; // How often new values are measured?

	PID(float p = 0, float i = 0, float d = 0, float imax = 0)
	{
		this->p = p;
		this->i = i;
		this->d = d;
		this->imax = imax;
		sample_time = 20 / 1000; // in seconds
	};

	float calculate(float difference)
	{
		i_sum += difference * sample_time;
		if (i_sum > imax)
		{
			i_sum = imax;
		}
		d_speed = (difference - last_error) / sample_time;
		return difference * p + i_sum * i + d_speed * d;
	};
};

#endif
