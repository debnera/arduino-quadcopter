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
#include <Wire.h>
#include <Servo.h>
#include <avr/wdt.h> // Watchdog timer

//#define MPU_DEBUG
// Motors
#define kMinThrottleToStabilize 100 // To prevent the stabilizer from affecting propeller speeds at low throttle.
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


// Control variables
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

#define ANGLE_CONTROL 0
#define RATE_CONTROL 1
#define RAW_CONTROL 2
int control_mode = ANGLE_CONTROL;

// Buffers
char rf_read_buffer[20];
int rf_read_buffer_len = 0;

// Bluetooth management variables and constants
bool bluetooth_ok;
unsigned long ping_offset = 0;
unsigned long previous_msg_received = 0;
int print_counter = 0;
int gyro_scale = 2000; // +-2000 degrees/s is default (250, 500, 1000 or 2000)

void dmpDataReady()
{
  // This is an interrupt routine for the mpu.
  mpu.mpuInterrupt = true;
}

void setup()
{
  wdt_enable(WDTO_2S); // Enables watchdog with 2 second timer
  Serial.begin(115200);
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

void loop()
{
  while (readBluetooth() == true)
  {
    if(parseCommand())
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

    // Get stuff from MPU
    cur_angles = mpu.getAngles() - offset_angles;
    cur_rates = mpu.getAngularRates();

    // Calculate new powers for motors
    motor_powers.setValues(0, 0, 0, 0);
    if (throttle > kMinThrottleToStabilize && control_mode != RAW_CONTROL)
    {
      if (control_mode == ANGLE_CONTROL)
      {
        Serial.print(F("ANGLE_CONTROL: "));
        target_rates = stabilizer.calculateRates(target_angles, cur_angles);
        motor_powers = stabilizer.calculatePowers(target_rates, cur_rates);
      }
      else if (control_mode == RATE_CONTROL)
      {
        Serial.print(F("RATE_CONTROL: "));
        motor_powers = stabilizer.calculatePowers(target_angles, cur_rates);
      }
      else if (control_mode == RAW_CONTROL)
      {
        Serial.print(F("RAW_CONTROL: "));
        motor_powers = stabilizer.rawPowers(target_angles);
      }
      motor_powers = motor_powers + throttle;
      motor_powers.setMinValues(kMinThrottleToStabilize - 20);
    }
    else
    {
      motor_powers = motor_powers + throttle;
    }
    setMotorPowers(motor_powers);

    // Print / debugging
    print_counter++;
    if ( print_counter % 40 == 0)
    {
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
      if (c == STX || rf_read_buffer_len > 19)
      {
        // Start of new command / buffer overflow - reset the buffer
        rf_read_buffer_len = 0;
      }
      else if (c == ETX)
      {
        // End of text received. Command is ready to be parsed.
        return true;
      }
      else
      {
        rf_read_buffer[rf_read_buffer_len++] = c;
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

bool parseCommand()
{
  bool success = false;
  int len = rf_read_buffer_len;
  char *command = rf_read_buffer;
  if (len > 0)
  {
    switch(command[0])
    {
      case ACK: // Ping ok
      {
        success = true; // We successfully received a command
        break;
      }
      case DC1: // Start
      {
        motor1.attach();
        motor2.attach();
        motor3.attach();
        motor4.attach();
        success = true; // We successfully received a command
        break;
      }
      case DC4: // Stop
      {
          stopMotors();
          success = true; // We successfully received a command
          break;
      }
      case 'y': // Yaw/pitch/roll
      {
        if (target_angles.fromArray(&command[0], len))
        {
          success = true; // We successfully received a command
        }
        break;
      }
      case 't': // Throttle
      {
        float value = parseFloat(&command[1], len - 1, &success);
        if (success == true)
        {
          throttle = value;
        }
        break;
      }
      case 'r': // Reset orientation (i.e. zeroes yaw-axis)
      {
        offset_angles.yaw = cur_angles.yaw + offset_angles.yaw;
        success = true;
      }
      case 'p': // P-value for roll/pitch PID
      {
        float value = parseFloat(&command[1], len - 1, &success);
        if (success == true)
        {
          stabilizer.changeP(value);
          Serial.print("New p value: ");
          Serial.println(value);
        }
        break;
      }
      case 'i': // I-value for roll/pitch PID
      {
        float value = parseFloat(&command[1], len - 1, &success);
        if (success == true)
        {
          stabilizer.changeI(value);
          Serial.print("New i value: ");
          Serial.println(value);
        }
        break;
      }
      case 'w': // P-value for yaw PID
      {
        float value = parseFloat(&command[1], len - 1, &success);
        if (success == true)
        {
          stabilizer.changeYP(value);
          Serial.print("New p value: ");
          Serial.println(value);
        }
        break;
      }
      case 'm': // Change the controller mode
      {
        if (len != 2)
        {
          Serial.println(F("Missing control mode index or too many numbers"));
          success = false;
          break;
        }
        else
        {
          int index = (int) command[1] - '0';
          success = true;
          if (index == ANGLE_CONTROL)
          {
            Serial.println(F("Changing to Angle Control mode"));
            control_mode = ANGLE_CONTROL;
          }
          else if (index == RATE_CONTROL)
          {
            Serial.println(F("Changing to Rate Control mode"));
            control_mode = RATE_CONTROL;
          }
          else if (index == RAW_CONTROL)
          {
            Serial.println(F("Changing to Raw Control mode"));
            control_mode = RAW_CONTROL;
          }
          else
          {
            Serial.println(F("Unknown control mode"));
            success = false;
          }
          break;
        }  
      }
    }
  }
  if (!success)
  {
    Serial.println("Invalid command.");
  }
  return success;
}

float parseFloat(char *str, int len, bool *success)
{
  /*
    Attempts to parse float from given string. Changes success to true if
    successful.
  */
  *success = true;
  if (len < 1)
  {
    *success = false;
    return 0;
  }
  float value = 0;
  bool decimal = false;
  int decimal_index = 1;
  bool is_negative = false;
  for (int i = 0; i < len; i++)
  {
    char c = str[i];
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
    else if (c == '-' && i == 0)
    {
      is_negative = true;
    }
    else
    {
      *success = false;
      break;
    }
  }
  if (is_negative) value = -value;
  return value;
}

void stopMotors()
{
  // Stops and detaches all motors.
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
  // Sets powers for all motors. Motors will not move if they are not attached.
	motor1.setPower(constrain((powers.x1 + kMinPwm), kMinPwm, kMaxPwm));
	motor2.setPower(constrain((powers.x2 + kMinPwm), kMinPwm, kMaxPwm));
	motor3.setPower(constrain((powers.x3 + kMinPwm), kMinPwm, kMaxPwm));
	motor4.setPower(constrain((powers.x4 + kMinPwm), kMinPwm, kMaxPwm));
}
