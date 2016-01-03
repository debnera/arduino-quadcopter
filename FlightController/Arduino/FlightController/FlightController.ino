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
#include <avr/wdt.h> // Watchdog timer

//#define MPU_DEBUG
// Motors
#define kMinThrottleToStabilize 70 // To prevent the stabilizer from affecting propeller speeds at low throttle.
#define kMaxThrottle 500 // Stabilizer is added on top of this.
#define kMinPwm 750 // Minimum pwm signal width - depends on ESC
#define kMaxPwm 1750 // Maximum pwm signal width - depends on ESC
#define motor_pin1 3
#define motor_pin2 4
#define motor_pin3 7
#define motor_pin4 8

// Bluetooth
#define bluetooth_baudrate 115200

// Special characters
const char STX = (char) 2; // Start of text (Starts command)
const char ETX = (char) 3; // End of text (Ends command)
const char ENQ = (char) 5; // Enquiry (for pinging bluetooth)
const char ACK = (char) 6; // Acknowledge

const char DC1 = (char) 17; // Start device
const char DC4 = (char) 20; // Stop device


Motor motor1 = Motor(motor_pin1);
Motor motor2 = Motor(motor_pin2);
Motor motor3 = Motor(motor_pin3);
Motor motor4 = Motor(motor_pin4);
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
// TODO: CircularBuffer is overkill for this purpose, a simple array would suffice.
CircularBuffer *bluetooth_read_cb;

// Bluetooth management variables and constants
bool bluetooth_ok;
unsigned long ping_offset = 0;
unsigned long previous_msg_received = 0;
int print_counter = 0;
int gyro_scale = 2000; // +-2000 degrees/s is default (250, 500, 1000 or 2000)

void dmpDataReady() {
    mpu.mpuInterrupt = true;
}

void setup() {
  wdt_enable(WDTO_2S); // Enables watchdog with 2 second timer
  Serial.begin(115200);
  bluetooth_read_cb = new CircularBuffer();
	mpu = MPU();
  Serial.println(F("Starting dmp..."));
  bool success = mpu.init();
  if(success)
  {
    Serial.println(F("DMP ready!"));
    attachInterrupt(0, dmpDataReady, RISING);
  }
  else
  {
    Serial.println(F("DMP failed to start!"));
  }
	throttle = 0;
  target_angles.setValues(0, 0, 0);
  mpu.setGyroScale(gyro_scale);
  offset_angles.setValues(0, 0.71, -2.7);
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


  while (readBluetooth() == true)
  {
    if(parseCommand(bluetooth_read_cb))
    {
      bluetooth_ok = true;
      previous_msg_received = millis();
      ping_offset = 0;
    }
  }

  if (bluetooth_ok)
  {
    if (millis() - previous_msg_received > (1000 + ping_offset))
    {
      ping_offset += 200;
      //Serial.println(ping_offset);
      sendPing();
    }
    if (millis() - previous_msg_received > 3000)
    {
      bluetooth_ok = false;
      Serial.println(F("Bluetooth lost"));
      stopMotors(); // also sets throttle to 0
      target_angles.setValues(0,0,0);
      target_rates.setValues(0,0,0);
    }
  }


  if (mpu.dataAvailable())
  {
    // TODO: Better logic for wdt_reset. If mpu returns crap, wdt is still
    // resetted.
    wdt_reset();
    if (mpu.fifoOverflow())
    {
      Serial.println(F("FIFO OVERFLOW!!!!"));
    }

    cur_angles = mpu.getAngles() - offset_angles;
    cur_rates = mpu.getAngularRates();



    motor_powers.setValues(0, 0, 0, 0);
    if (throttle > kMinThrottleToStabilize)
    {
      target_rates = stabilizer.calculateRates(target_angles, cur_angles);
      //target_rates = target_angles;
      motor_powers = stabilizer.calculatePowers(target_rates, cur_rates);
      motor_powers = motor_powers + throttle;
      motor_powers.setMinValues(kMinThrottleToStabilize - 20);
    }
    else
    {
      motor_powers = motor_powers + throttle;
    }

    setMotorPowers(motor_powers);

    print_counter++;
    if ( print_counter % 40 == 0)
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
      Serial.print(cur_angles.roll);
      Serial.print("\ttarget\t");
      Serial.print(target_angles.yaw);
      Serial.print("\t");
      Serial.print(target_angles.pitch);
      Serial.print("\t");
      Serial.println(target_angles.roll);/*
      Serial.print("rates\t");
      Serial.print(cur_rates.yaw);
      Serial.print("\t");
      Serial.print(cur_rates.pitch);
      Serial.print("\t");
      Serial.println(cur_rates.roll);*/
      //Serial.println();
    }
    if ( print_counter == 2000)
    {
      //throttle = 100;
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
  if (Serial.available() > 63) // Buffer size is 64 bytes.
  {
    // Bluetooth is flooded. This should not happen unless our code is really
    // slow, or the pc is sending way too much data.
    Serial.println(F("Bluetooth overflow!! Too much data or too slow code."));
    while (Serial.available()) Serial.read(); // Clear buffer
  }
  else
  {
    while (Serial.available())
  	{

      char c = (char)Serial.read();
      //Serial.print(c);
      if (c == STX)
      {
        // Start of new command - ignore all possible gibberish before it.
        bluetooth_read_cb->reset();
        //Serial.println("STX received");
      }
      bool cb_overflow = bluetooth_read_cb->write(c);
      if (cb_overflow)
      {
        // Buffer overflown before ETX was received.
        //bluetooth.println("Command buffer overflow!! - no ETX received.");
      }
      if (c == ETX)
      {
        // End of text received. Command is ready to be parsed.
        //Serial.println("ETX received");
        return true;
      }
  	}
  }
  return false; // No full command received
}

void sendPing()
{
  /*
  Sends enquiry to the controller, so it knows to immediately respond back.
  This is used to check the status of the connection.
  */
  //Serial.print(STX);
  Serial.print(ENQ);
  //Serialprint(ETX);
}

bool parseCommand(CircularBuffer *buffer)
{
  bool success = false;
  int len = buffer->length();
  if (len < 2) return false;
  //Serial.print("Received: ");
  char *command = (char*)malloc(len);
  for (int i = 0; i < len; i++)
  {
    command[i] = buffer->read();
    //Serial.print(command[i]);
  }
  if (command[0] == STX && len > 1)
  {
    switch(command[1])
    {
      case ACK:
        success = true; // We successfully received a command
        break;
      case DC1:
        //bluetooth.println("Starting engines");
        motor1.attach();
        motor2.attach();
        motor3.attach();
        motor4.attach();
        success = true; // We successfully received a command
        break;
      case DC4:
        stopMotors();
        //bluetooth.println("Killing engines");
        success = true; // We successfully received a command
        break;
      case 'y':
        //bluetooth.println("Angles received");
        if (target_angles.fromArray(&command[1], len - 2)) // len - (STX + ETX)
        {
          success = true; // We successfully received a command
          //bluetooth.println(target_angles.toString());
        }
        break;
      case 'p': // p-value for roll/pitch PID
      {
        if (len < 4) return false; // No value given (STX + 't' + ETX)
        float value = 0;
      	bool decimal = false;
      	int decimal_index = 1;
      	for (int i = 2; i < len - 1; i++)
      	{
      		char c = command[i];
      		if ('0' <= c && c <= '9')
      		{
      			if (decimal == false)
      				value = value * 10 + c - '0';
      			else
      			{
      				value += (c - '0') / pow(10, decimal_index);
      				decimal_index++;
      			}
      		}
      		else if ((c == '.' || c == ',') && decimal == false)
      			decimal = true;
      		else
      			return false;
      	}
        success = true;
        stabilizer.changeP(value);
        Serial.print("New p value: ");
        Serial.println(value);
        break;
      }
      case 'i': // p-value for roll/pitch PID
      {
        if (len < 4) return false; // No value given (STX + 't' + ETX)
        float value = 0;
      	bool decimal = false;
      	int decimal_index = 1;
      	for (int i = 2; i < len - 1; i++)
      	{
      		char c = command[i];
      		if ('0' <= c && c <= '9')
      		{
      			if (decimal == false)
      				value = value * 10 + c - '0';
      			else
      			{
      				value += (c - '0') / pow(10, decimal_index);
      				decimal_index++;
      			}
      		}
      		else if ((c == '.' || c == ',') && decimal == false)
      			decimal = true;
      		else
      			return false;
      	}
        success = true;
        stabilizer.changeI(value);
        Serial.print("New i value: ");
        Serial.println(value);
        break;
      }
      case 't':
      {
        //bluetooth.println("Throttle received");
        if (len < 4) return false; // No value given (STX + 't' + ETX)
        int x = 0;
        for (int i = 2; i < len - 1; i++)
        {
          int cmd = (int)command[i] - (int)'0';
          if (cmd >= 0 && cmd <= 9)
          {
            x = 10*x + cmd;
          }
          else if (i == 2 && command[i] == '-') // Negative value
          {
            x = -1;
            break;
          }
          else return false; // Invalid character encountered
        }
        //if (x > kMaxThrottle) x = kMaxThrottle;
        throttle = x;
        success = true; // We successfully received a command
        //bluetooth.println(throttle);
        break;
      }
    }
  }
  //Serial.println();
  free(command);
  return success;
}


void stopMotors()
{
  throttle = 0;
  motor1.setPower(kMinPwm);
  motor2.setPower(kMinPwm);
  motor3.setPower(kMinPwm);
  motor4.setPower(kMinPwm);
  motor1.detach();
  motor2.detach();
  motor3.detach();
  motor4.detach();
}

void setMotorPowers(Vector4 powers)
{
	motor1.setPower(constrain((powers.x1 + kMinPwm), kMinPwm, kMaxPwm));
	motor2.setPower(constrain((powers.x2 + kMinPwm), kMinPwm, kMaxPwm));
	motor3.setPower(constrain((powers.x3 + kMinPwm), kMinPwm, kMaxPwm));
	motor4.setPower(constrain((powers.x4 + kMinPwm), kMinPwm, kMaxPwm));
}
