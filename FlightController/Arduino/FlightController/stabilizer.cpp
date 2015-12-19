//
//
//

#include "stabilizer.h"


Stabilizer::Stabilizer()
{
	pid_yaw_rate.p = 2.5;
	//pid_yaw_rate.i = 1;
	pid_yaw_rate.d = 0;
	pid_yaw_rate.imax = 50;

	pid_pitch_rate.p = 0.7;
	//pid_pitch_rate.i = 1;
	pid_pitch_rate.d = 0;
	pid_pitch_rate.imax = 50;

	pid_roll_rate.p = 0.7;
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

Vector4 Stabilizer::calculatePowers(Angles target_rates, Angles cur_rates)
{
	Angles differences = Angles();
	// TODO Add i and d terms.
	differences.yaw = (target_rates.yaw - cur_rates.yaw) * pid_yaw_rate.p;
	differences.pitch = (target_rates.pitch - cur_rates.pitch) * pid_pitch_rate.p;
	differences.roll = (target_rates.roll - cur_rates.roll) * pid_roll_rate.p;

	Vector4 motor_powers = Vector4(0, 0, 0, 0);
	// TODO Calculate yaw
	motor_powers.x1 = + differences.roll - differences.pitch + differences.yaw; // Front right
	motor_powers.x2 = - differences.roll + differences.pitch + differences.yaw; // Back left
	motor_powers.x3 = - differences.roll - differences.pitch - differences.yaw; // Front left
	motor_powers.x4 = + differences.roll + differences.pitch - differences.yaw; // Back right
	return motor_powers;
}

Angles Stabilizer::calculateRates(Angles target_angles, Angles cur_angles)
{
	Angles differences = Angles();
	// TODO Add i and d terms.
	differences.yaw = (cur_rates.yaw - target_rates.yaw) * pid_yaw_rate.p;
	differences.pitch = (cur_rates.pitch - target_rates.pitch) * pid_pitch_rate.p;
	differences.roll = (cur_rates.roll - target_rates.roll) * pid_roll_rate.p;
	return Angles(0, 0, 0);
}
