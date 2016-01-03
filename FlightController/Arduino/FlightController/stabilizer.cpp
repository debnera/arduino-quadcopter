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

void Stabilizer::changeP(float p)
{
		pid_pitch_rate.p = p;
		pid_roll_rate.p = p;
}
void Stabilizer::changeI(float p)
{
		pid_pitch_rate.i = p;
		pid_roll_rate.i = p;
}

Vector4 Stabilizer::calculatePowers(Angles target_rates, Angles cur_rates)
{
	Angles differences = Angles();
	float min = -500;
	float max = 500;
	differences.yaw = constrain(pid_yaw_rate.calculate(target_rates.yaw - cur_rates.yaw), min, max);
	differences.pitch = constrain(pid_pitch_rate.calculate(target_rates.pitch - cur_rates.pitch), min, max);
	differences.roll = constrain(pid_roll_rate.calculate(target_rates.roll - cur_rates.roll), min, max);

	Vector4 motor_powers = Vector4(0, 0, 0, 0);
	motor_powers.x3 = + differences.roll + differences.pitch - differences.yaw; // Front left
	motor_powers.x2 = + differences.roll - differences.pitch + differences.yaw; // Back left
	motor_powers.x1 = - differences.roll + differences.pitch + differences.yaw; // Front right
	motor_powers.x4 = - differences.roll - differences.pitch - differences.yaw; // Back right
	return motor_powers;
}

Angles Stabilizer::calculateRates(Angles target_angles, Angles cur_angles)
{
	Angles differences = Angles();
	float min = -250;
	float max = 250;
	differences.yaw = constrain(pid_yaw_stab.calculate(target_angles.yaw - cur_angles.yaw), min, max);
	differences.pitch = constrain(pid_pitch_stab.calculate(target_angles.pitch - cur_angles.pitch), min, max);
	differences.roll = constrain(pid_roll_stab.calculate(target_angles.roll - cur_angles.roll), min, max);
	return differences;
}

/*
float Stabilizer::constrain(float val, float min, float max)
{
	if (val < min) return min;
	else if (val > max) return max;
	return val;
}

int Stabilizer::constrain(int val, int min, int max)
{
	if (val < min) return min;
	else if (val > max) return max;
	return val;
}
*/
