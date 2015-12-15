/*
Name:		FlightController.ino
Created:	8/24/2015 1:37:29 PM
Author:	Anton
*/

#include "vector4.h"
#include "stabilizer.h"
#include "serial_communicator.h"
#include "pid.h"
#include "mpu.h"
#include "motor.h"
#include <Wire.h>
#include <Servo.h>
#include <SoftwareSerial.h>


// Motors
// Throttle is in range [0, max_pwm - min_pwm].
#define kMinThrottleToStabilize 100 // To prevent the stabilizer from affecting propeller speeds at low throttle.
#define kMaxThrottleBeforeStabilize 1100 // Stabilizer needs some room to work even at max throttle.
#define kAbsoluteMaxThrottle 2000 // This can be used to limit the maximum throttle after stabilizer.
#define min_pwm 750 // Minimum pwm signal width - depends on ESC
#define max_pwm 2750 // Maximum pwm signal width - depends on ESC
#define motor_pin1 3
#define motor_pin2 9
#define motor_pin3 10
#define motor_pin4 11

// Bluetooth
#define bluetooth_rx_pin 7 
#define bluetooth_tx_pin 8
#define bluetooth_baudrate 38400


Motor *motors;
SerialCommunicator *bluetooth;
Angles target_angles;
Angles cur_angles;
Angles target_rates;
Angles cur_rates;
Vector4 motor_powers;
int throttle;
Stabilizer *stabilizer;
MPU *mpu;

// Bluetooth management variables and constants
const String kPingRequest = "ping";
const String kPingResponse = "pingok";
int ping_timer;
int connection_timeout_timer;
bool isBluetoothConnected;

void setup() {
	mpu = new MPU(bluetooth);

	bluetooth = new SerialCommunicator(bluetooth_rx_pin, bluetooth_tx_pin, bluetooth_baudrate);
	isBluetoothConnected = false;
	ping_timer = 0;
	connection_timeout_timer = 0; // time in ms
	Motor new_motors[4] = { Motor(motor_pin1, "M1: Top-right", min_pwm, max_pwm),
							Motor(motor_pin2, "M2: Bottom-left", min_pwm, max_pwm),
							Motor(motor_pin3, "M3: Top-left", min_pwm, max_pwm),
							Motor(motor_pin4, "M4: Bottom-right", min_pwm, max_pwm) };
	motors = new_motors;
	throttle = 0;

}

// the loop function runs over and over again until power down or reset
void loop() {

	// Get and parse commands from controller.
	bluetooth->readFromSerial();
	String command = bluetooth->getCompletedCommand();
	if (command.length() > 0)
	{
		parseCommand(command);
	}
	if (throttle > kMaxThrottleBeforeStabilize)
	{
		throttle = kMaxThrottleBeforeStabilize;
	}

	// Check connection status
	if (isBluetoothConnected == false)
	{
		// We lost connection - let's make sure the propellers don't spin.
		target_angles.setValues(0, 0, 0);
		throttle = 0;
	}

	// Get values from MPU
	Angles mpu_angles = mpu->getAngles();
	cur_angles.setValues(mpu_angles.yaw, mpu_angles.pitch, mpu_angles.roll);
	Angles mpu_rates = mpu->getAngularRates();
	cur_rates.setValues(mpu_rates.yaw, mpu_rates.pitch, mpu_rates.roll);

/*
	// Calculate values from stabilizer
 
	if (throttle >= kMinThrottleToStabilize)
	{
		target_rates = stabilizer->calculateRates(target_angles, cur_angles);
		motor_powers = stabilizer->calculatePowers(target_rates, cur_rates);
	}
	else
	{
		motor_powers = Vector4(throttle, throttle, throttle, throttle);
	}

	setMotorPowers(motor_powers);
 */

}

void parseCommand(String command)
{
	// TODO plan:	1. Potential angles/throttle -> parse angles -> parse throttle
	//				2. Other commands
	bool isAngles = target_angles.fromString(command);

}

void setMotorPowers(Vector4 powers)
{
	motors[0].setPower(powers.val1);
	motors[1].setPower(powers.val2);
	motors[2].setPower(powers.val3);
	motors[3].setPower(powers.val4);
}


