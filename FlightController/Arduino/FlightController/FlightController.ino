/*
Name:		FlightController.ino
Created:	8/24/2015 1:37:29 PM
Author:	Anton
*/

#include "vector4.h"
#include "stabilizer.h"
#include "pid.h"
#include "mpu.h"
#include "motor.h"
#include "circular_buffer.h"
#include <Wire.h>
#include <Servo.h>
#include <SoftwareSerial.h>
#include <avr/wdt.h> // Watchdog timer

//#define MPU_DEBUG
// Motors
#define kMinThrottleToStabilize 70 // To prevent the stabilizer from affecting propeller speeds at low throttle.
#define kMaxThrottle 500 // Stabilizer is added on top of this.
#define kMinPwm 750 // Minimum pwm signal width - depends on ESC
#define kMaxPwm 1750 // Maximum pwm signal width - depends on ESC
#define motor_pin1 3
#define motor_pin2 9
#define motor_pin3 10
#define motor_pin4 11

// Bluetooth
#define bluetooth_rx_pin 7
#define bluetooth_tx_pin 8
#define bluetooth_baudrate 38400

// Special characters
#define STX 2 // Start of text (Starts command)
#define ETX 3 // End of text (Ends command)
#define ENQ 5 // Enquiry (for pinging bluetooth)
#define ACK 6 // Acknowledge

#define DC1 17 // Start device
#define DC4 20 // Stop device


Motor *motors;
SoftwareSerial bluetooth = SoftwareSerial(bluetooth_rx_pin, bluetooth_tx_pin, false); // false for non-inverted logic
Angles target_angles;
Angles cur_angles;
Angles offset_angles;
Angles target_rates;
Angles cur_rates;
Vector4 motor_powers;
int throttle;
Stabilizer stabilizer;
MPU mpu;

// Buffers
CircularBuffer bluetooth_read_cb;

// Bluetooth management variables and constants
const String kPingRequest = "ping";
const String kPingResponse = "pingok";
int ping_timer;
int connection_timeout_timer;
bool isBluetoothConnected;
int print_counter = 0;
int gyro_scale = 2000; // +-2000 degrees/s is default (250, 500, 1000 or 2000)

void dmpDataReady() {
    mpu.mpuInterrupt = true;
}

void setup() {
  wdt_enable(WDTO_2S); // Enables watchdog with 2 second timer
  Serial.begin(115200);
  bluetooth.begin(bluetooth_baudrate);
	mpu = MPU();
  Serial.println(F("Starting dmp..."));
  bool success = mpu.init();
  if(success)
  {
    Serial.println(F("DMP ready! Waiting for first interrupt..."));
    attachInterrupt(0, dmpDataReady, RISING);
  }
  else
  {
    Serial.println(F("-----DMP failed to start!-----"));
  }
	isBluetoothConnected = false;
	ping_timer = 0;
	connection_timeout_timer = 0; // time in ms
	Motor new_motors[4] = { Motor(motor_pin1, "M1: Top-right"),
            							Motor(motor_pin2, "M2: Bottom-left"),
            							Motor(motor_pin3, "M3: Top-left"),
            							Motor(motor_pin4, "M4: Bottom-right") };
	motors = new_motors;
	throttle = 70;
  mpu.setGyroScale(gyro_scale);
}

// the loop function runs over and over again until power down or reset
void loop() {

	/*// Get and parse commands from controller.
	bluetooth->readFromSerial();
	String command = bluetooth->getCompletedCommand();
	if (command.length() > 0)
	{
		parseCommand(command);
	}

	// Check connection status
	if (isBluetoothConnected == false)
	{
		// We lost connection - let's make sure the propellers don't spin.
		target_angles.setValues(0, 0, 0);
		throttle = 0;
	}
  */
	// Get values from MPU
  wdt_reset(); // Reset watchdog timer
  while (readBluetooth() == true)
  {
    parseCommand(bluetooth_read_cb);
  }
  if (mpu.dataAvailable())
  {
    if (mpu.fifoOverflow())
    {
      Serial.println("---------------FIFO OVERFLOW!!!!-----------");
      bluetooth.print("---------------FIFO OVERFLOW!!!!-----------");
    }

    cur_angles = mpu.getAngles() - offset_angles;
    cur_rates = mpu.getAngularRates();

    target_angles.setValues(0, 0, 0);

    motor_powers.setValues(0, 0, 0, 0);
    if (throttle > kMinThrottleToStabilize)
    {
      target_rates = stabilizer.calculateRates(target_angles, cur_angles);
      motor_powers = stabilizer.calculatePowers(target_rates, cur_rates);
    }
    motor_powers = motor_powers + throttle;

    print_counter++;
    if ( print_counter % 20 == 0)
    {
      //Serial.println(motor_powers.x1);

      Serial.print("Powers\t");
      Serial.print(motor_powers.x1);
      Serial.print("\t");
      Serial.print(motor_powers.x2);
      Serial.print("\t");
      Serial.print(motor_powers.x3);
      Serial.print("\t");
      Serial.print(motor_powers.x4);
      Serial.print("\t");
      Serial.print("ypr\t");
      Serial.print(cur_angles.yaw);
      Serial.print("\t");
      Serial.print(cur_angles.pitch);
      Serial.print("\t");
      Serial.println(cur_angles.roll);
      /*Serial.print("rates\t");
      Serial.print(cur_rates.yaw);
      Serial.print("\t");
      Serial.print(cur_rates.pitch);
      Serial.print("\t");
      Serial.println(cur_rates.roll);*/
      //Serial.println();
    }
    if ( print_counter == 2000)
    {
      throttle = 100;
      offset_angles.yaw = cur_angles.yaw + offset_angles.yaw;
    }
  }
}

bool readBluetooth()
{
  /*
   Reads pending data from bluetooth. If an ETX is encountered, true is
   returned without reading the rest of the data. Otherwise all data will
   be read in to the buffer and false is returned.
  */
  if (bluetooth.overflow())
  {
    // Bluetooth is flooded. This should not happen unless our code is really
    // slow, or the pc is sending way too much data.
    bluetooth.println("Bluetooth overflow!! Too much data or too slow code.");
    bluetooth.println("Clearing bluetooth.");
    while (bluetooth.available()) bluetooth.read(); // Clear buffer
  }
  else
  {
    while (bluetooth.available())
  	{

      char c = bluetooth.read();
      Serial.print(c);
      if (c == (char)STX)
      {
        // Start of new command - ignore all possible gibberish before it.
        bluetooth_read_cb.reset();
      }
      bool cb_overflow = bluetooth_read_cb.write(c);
      if (cb_overflow)
      {
        // Buffer overflown before ETX was received.
        bluetooth.println("Command buffer overflow!! - no ETX received.");
      }
      if (c == (char)ETX)
      {
        // End of text received. Command is ready to be parsed.
        return true;
      }
  	}
  }
  return false; // No full command received
}

void parseCommand(CircularBuffer buffer)
{
	if (buffer.length() > 0)
  {
    Serial.print("Received: ");
    char *command = (char*)malloc(buffer.length());
    for (int i = 0; i < buffer.length(); i++)
    {
      command[i] = buffer.read();
      Serial.print(command[i]);
    }
    Serial.println();
  }
}

void stopMotors()
{
  motors[0].setPower(-1);
  motors[1].setPower(-1);
  motors[2].setPower(-1);
  motors[3].setPower(-1);
}

void setMotorPowers(Vector4 powers)
{
	motors[0].setPower(constrain((powers.x1 + kMinPwm), kMinPwm, kMaxPwm));
	motors[1].setPower(constrain((powers.x2 + kMinPwm), kMinPwm, kMaxPwm));
	motors[2].setPower(constrain((powers.x3 + kMinPwm), kMinPwm, kMaxPwm));
	motors[3].setPower(constrain((powers.x4 + kMinPwm), kMinPwm, kMaxPwm));
}
