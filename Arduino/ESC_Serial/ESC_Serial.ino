#include <Servo.h>
#include "Motor.h"

#define MIN_SIGNAL 700 // Default 544us (Typical ESC-range: 700us-2000us)
#define MAX_SIGNAL 2000  // Default 2400us
#define MOTOR1_PIN 3
#define MOTOR2_PIN 5
#define MOTOR3_PIN 6
#define MOTOR4_PIN 9



Motor motor1 = {MOTOR1_PIN, "Motor 1"};
Motor motor2 = {MOTOR2_PIN, "Motor 2"};
Motor motor3 = {MOTOR3_PIN, "Motor 3"};
Motor motor4 = {MOTOR4_PIN, "Motor 4"};
Motor motors[4] = {motor1, motor2, motor3, motor4};

int speed = 0;


void setup()
{
  Serial.begin(115200);
  speed = 0; // Set default speed
  Serial.println("Starting speed set to 0");
  String a = "PWM range is set to ";
  a += MIN_SIGNAL;
  a += "uS - ";
  a += MAX_SIGNAL;
  a += "uS";
  Serial.println(a);
  Serial.println("Motors are offline, waiting for input - Attach with 'a', detach with 'd'");
}

void setSpeed(Motor motor, int angle) 
{
  if (motor.servo.attached() == true)
  {
    motor.servo.write(angle);
  }
    
}

void attachMotor(Motor motor)
{
  if (motor.servo.attached() == false)
  {
    motor.servo.attach(motor.pin, MIN_SIGNAL, MAX_SIGNAL); // Attach pin, set min and max pwm range
    Serial.print(motor.description);
    Serial.println(" attached");
    setSpeed(motor, speed); 
  }
}

void detachMotor(Motor motor)
{
  if (motor.servo.attached() == false)
  {
    motor.servo.detach();
    Serial.print(motor.description);
    Serial.println(" detached");
  }
}

String readSerial()
{
  unsigned int maxDelay = 50000; // 50ms
  unsigned int minDelay = 100; // 0.5ms
  unsigned int timeDelayed = 0;
  String input;
  char c = 0;
  // Incase we have multiple commands in buffer (separated with '\n'), return the first command found.
  while (timeDelayed < maxDelay)
  {
    while (Serial.available() > 0)
    {
      c = Serial.read();
      input += c;
    }
    if (input.endsWith(String('\n')) == true) // End of command received
    {
      break;
    }
    delayMicroseconds(minDelay); // Wait for all the data to be received.
    timeDelayed += minDelay;
  }
  Serial.print("Delay: ");
  Serial.println(timeDelayed);
  
  return input;
}

void parseThrottle(String input)
{
  int value = input.toInt();
  Serial.println(value);
}

void loop()
{
  // If there is incoming value
  if (Serial.available() > 0)
  {
    String command = readSerial();
    Serial.println("received:");
    Serial.print(command);
    Serial.print('\n');
    //parseThrottle(command);
  }
}
