//
//
//

#include "stabilizer.h"


Stabilizer::Stabilizer(int max_motor_power)
{
	pid_yaw_rate.p = 2.5;
	//pid_yaw_rate.i = 1;
	pid_yaw_rate.d = 0;
	pid_yaw_rate.imax = 50;

	pid_pitch_rate.p = 0.7;
	//pid_pitch_rate.i = 1;
	pid_pitch_rate.d = 0;
	pid_pitch_rate.imax = 50;

	pid_roll_rate.p ) 0.7;
	//pid_roll_rate.i = 1;
	pid_roll_rate.d = 0;
	pid_roll_rate.imax = 50;

	pid_yaw_stab.p = 10;
	pid_yaw_stab.i = 0;
	pid_yaw_stab.d = 0;
	pid_pitch_stab.p = 4.5;
	pid_pitch_stab.i = 0;
	pid_pitch_stab.d = 0;
	pid_roll_stab.p = 4.5;
	pid_roll_stab.i = 0;
	pid_roll_stab.d = 0;
}

Angles Stabilizer::calculateRates(Angles target_angles, Angles cur_angles)
{
	// TODO - implement me
	return Angles(0, 0, 0);
}

Vector4 Stabilizer::calculatePowers(Angles target_rates, Angles cur_rates)
{
	// TODO - implement me
	return Vector4(0, 0, 0, 0);
}
