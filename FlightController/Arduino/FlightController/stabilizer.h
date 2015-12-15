// stabilizer.h

#ifndef _STABILIZER_h
#define _STABILIZER_h

#if defined(ARDUINO) && ARDUINO >= 100
#include "arduino.h"
#else
#include "WProgram.h"
#endif

#include "angles.h"
#include "pid.h"
#include "vector4.h"

class Stabilizer
{
private:
	int max_power;
	PID pid_yaw_rate;
	PID pid_pitch_rate;
	PID pid_roll_rate;
	PID pid_yaw_stab;
	PID pid_pitch_stab;
	PID pid_roll_stab;
public:
	Stabilizer(int max_motor_power);
	Angles calculateRates(Angles target_angles, Angles cur_angles);
	Vector4 calculatePowers(Angles target_rates, Angles cur_rates);
};

#endif

