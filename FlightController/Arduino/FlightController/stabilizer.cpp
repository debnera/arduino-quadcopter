// 
// 
// 

#include "stabilizer.h"


Stabilizer::Stabilizer(int max_motor_power)
{
	// TODO - implement me
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