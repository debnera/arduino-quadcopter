// stabilizer.h

#ifndef _STABILIZER_h
#define _STABILIZER_h

#if defined(ARDUINO) && ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif

#include "angles.h"
#include "pid.h"
#include "vector4.h"

class Stabilizer
{
private:
	PID pid_yaw_rate;
	PID pid_pitch_rate;
	PID pid_roll_rate;
	PID pid_yaw_stab;
	PID pid_pitch_stab;
	PID pid_roll_stab;

public:
	Stabilizer();
	Angles calculateRates(Angles target_angles, Angles cur_angles);
	Vector4 calculatePowers(Angles target_rates, Angles cur_rates);
	Vector4 rawPowers(Angles target_powers);
	void changeP(float p);
	void changeYP(float p);
	void changeI(float i);
};

#endif
