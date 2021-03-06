/* Author: Anton Debner
 * My first quadcopter project 2015
 */

#include <SoftwareSerial.h>
#include <Servo.h>
#include "Motor.h"


#define MIN_SIGNAL 750 // Default 544us (Typical ESC-range: 700us-2000us)
#define MAX_SIGNAL 2750  // Default 2400us
#define MOTOR1_PIN 3
#define MOTOR2_PIN 9
#define MOTOR3_PIN 10
#define MOTOR4_PIN 11
#define BAUDRATE 38400
SoftwareSerial softSerial(7,8); // RX, TX

Motor motor1 = {MOTOR1_PIN, "Motor 1"};
Motor motor2 = {MOTOR2_PIN, "Motor 2"};
Motor motor3 = {MOTOR3_PIN, "Motor 3"};
Motor motor4 = {MOTOR4_PIN, "Motor 4"};
Motor motors[4] = {motor1, motor2, motor3, motor4};

class Timer
{
  private:
    unsigned long prevTime = 0;
    unsigned long interval = 0;
  public:
    unsigned long getDifference()
    {
      return millis() - prevTime;
    }
    unsigned long getInterval()
    {
      return millis() - interval;
    }
    void updateTime()
    {
      prevTime = millis();
      interval = millis();
    }
    void updateInterval()
    {
      interval = millis();
    }
};


int speed = 0;
boolean useAngles = false; // False to use servo.writeMicroseconds(time) instead of servo.write(angle). Write seems to ignore manually set pwm range.
boolean printPing = false; // True to print confirmation of successfull pinging
boolean conOK = false; // Are we still connected to the controller?
unsigned int conTimeout = 5000; // Time(ms) after the connection is assumed to be lost.
unsigned int conInterval = 1000; // Interval(ms) in which the connection is checked.
String conQueryMessage = "ping"; // The message which the controller regocnizes and answers to
String conOkMessage = "pingok"; // The controllers expected response to conQuery

Timer conTimer; // Timer for tracking connection timeout
Timer receiveTimer; // Timer for delayed reading of incoming messages



void setup()
{
  conTimer.updateTime();
  softSerial.begin(BAUDRATE);
  speed = 0; // Set default speed
  softSerial.println("Starting ESC_Bluetooth sketch");
  softSerial.println("Starting speed set to 0");
  String a = "PWM range is set to ";
  a += MIN_SIGNAL;
  a += "uS - ";
  a += MAX_SIGNAL;
  a += "uS";
  softSerial.println(a);
  softSerial.println("Motors are offline, waiting for input - Attach with 'a', detach with 'd'");
}

void setSpeed(Motor motor, int angle) 
{
  if (motor.servo.attached() == true)
  {
    if (useAngles == true)
    {
      motor.servo.write(angle);
    }
    else
    {
      int speedms = map(angle, 0, 180, MIN_SIGNAL, MAX_SIGNAL);
      motor.servo.writeMicroseconds(speedms);
    } 
  } 
}

void setMicroseconds(Motor motor, int value)
{
  // Set speed in microseconds.
  if (motor.servo.attached() == true)
  {
      motor.servo.writeMicroseconds(value);
  } 
}

void attachMotor(Motor motor)
{
  if (motor.servo.attached() == false)
  {
    motor.servo.attach(motor.pin, MIN_SIGNAL, MAX_SIGNAL); // Attach pin, set min and max pwm range
    softSerial.print(motor.description);
    softSerial.println(" attached");
    setSpeed(motor, speed); 
  }
}

void detachMotor(Motor motor)
{
  if (motor.servo.attached() == true)
  {
    motor.servo.detach();
    softSerial.print(motor.description);
    softSerial.println(" detached");
  }
}

String readsoftSerial()
{
  // Reads the incoming string from the softSerial. Because software serial is relatively slow, this function
  // attempts to wait (delay) until the entire message is received, ending in end of line ('\n').

  // This function stops reading when it either receives '\n' or the maxDelay is exceeded.
  // Delays are in microseconds   (1000uS = 1ms;    1 000 000uS = 1s)
  unsigned int maxDelay = 50000; // 50ms 
  unsigned int minDelay = 100; // 0.1ms
  receiveTimer.updateTime();
  String input;
  char c = 0;
  
  // Incase we have multiple commands in buffer (separated with '\n'), return the first command found.
  while (receiveTimer.getDifference() < maxDelay)
  {
    while (softSerial.available() > 0)
    {
      c = softSerial.read();
      input += c;
    }
    if (input.endsWith(String('\n')) == true) // End of command received
    {
      break;
    }
    delayMicroseconds(minDelay); // Wait for all the data to be received.
  }
  
  //softSerial.print("Delay: ");
  //softSerial.println(timeDelayed);
  return input;
}

void parseThrottle(String input)
{
  if (input.length() > 3)
  {
    softSerial.println("INPUT ERROR: Maximum of 3 digits allowed (0-180)");
    return;
  }
  for(int i = 0; i < input.length(); i++)
  {
    if (input[i] < '0' || input[i] > '9')
    {
      softSerial.println("INPUT ERROR: Not a valid number");
      return;
    }
  }
  int value = input.toInt();
  if (value < 0 || value > 180)
  {
    softSerial.println("INPUT ERROR: Value is not in range 0-180");
    return;
  }
  softSerial.print("Changing throttle to ");
  softSerial.println(value);
  speed = value;
}

void parseCommands(String input)
{
  // Track connection status
  if (input.equals("pingok"))
  {
    conTimer.updateTime(); // Connection is ok, reset the timeout timer
    conOK = true;
    if (printPing == true) softSerial.println("Ping received - connection OK");
  }
  else if (input.equals("p"))
  {
    printPing = !printPing;
    softSerial.print("PrintPing == ");
    softSerial.println(printPing);
  }
  // Change speed writing mode
  else if (input.equals("q"))
  {
    useAngles = !useAngles;
    softSerial.print("Use angles == ");
    softSerial.println(useAngles);
  }
  // Attach
  else if (input[0] == 'a')
  {
    int count = 0;
    for (unsigned int i = 1; i < input.length(); i++)
    {
      unsigned int index = input[i] - '0';
      index -= 1; // We want the indexes to be in range 0-3 instead of 1-4.
      if (index < 4)
      {
        Motor motor = motors[index]; 
        attachMotor(motor); 
      }
    }
  }
  // Detach
  else if (input[0] == 'd')
  {
    int count = 0;
    for (unsigned int i = 1; i < input.length(); i++)
    {
      unsigned int index = input[i] - '0';
      index -= 1; // We want the indexes to be in range 0-3 instead of 1-4.
      if (index < 4)
      {
        Motor motor = motors[index]; 
        detachMotor(motor);
      }
    }
  }
  // Set speed in microseconds. This can be used to find out the limits of the ESC
  // before manually editing the pwm signal ranges in this sketch.
  else if (input[0] == 'm')
  {
    input[0] = '0'; // Let's make the String in to an int.
    int value = input.toInt();
    if (value >= 0 && value <= 5000)
    {
      softSerial.print("Setting motor speed in microseconds to ");
      softSerial.println(value);
      for (unsigned int i = 0; i < 4; i++)
      {
        setMicroseconds(motors[i], value);
      }
    }
    else softSerial.println("Invalid microsecond value (0-5000)");
  }
  else
  {
    softSerial.println(input);
    softSerial.println("Invalid command. Attach motors with 'a' and detach with 'd'");
    softSerial.println("Example: 'a24' - attaches motors 2 and 4");
  }
}

boolean isMaybeNumber(String input)
{
  // Checks if the first symbol in given string is a number.
  // Doesn't guarantee that the entire string is a number.
  if (input[0] < '0' || input[0] > '9')
  {
    return false;
  }
  return true;
}

void sendPingQuery()
{
  // Sends the connection query message. The receival of
  softSerial.println(conQueryMessage);
  if (conTimer.getDifference() > conTimeout)
  {
    // We have now lost connection to the controller
    softSerial.println("Connection lost");
    conOK = false;
    for (unsigned int i = 0; i < 4; i++)
    {
      // Let's detach every motor
      detachMotor(motors[i]);
    }
  }
}

void loop()
{
  if (conTimer.getInterval() > conInterval)
  {
    conTimer.updateInterval();
    sendPingQuery();
  }
  // If there is incoming value
  if (softSerial.available() > 0)
  {
    String command = readsoftSerial(); // Using String instead of char[] for added functionality (less memory effective)
    command.toLowerCase();
    command.trim();
    if (isMaybeNumber(command) == true) // No commands begin with a number, maybe it's a throttle input?
    {
      parseThrottle(command);
      for (unsigned int i = 0; i < 4; i++)
      {
        setSpeed(motors[i], speed);
      }
    }
    else
    {
      parseCommands(command);
    }
  }
}




