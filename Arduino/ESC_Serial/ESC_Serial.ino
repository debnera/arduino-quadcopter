#include <Servo.h>
#include "Motor.h"

#define MIN_SIGNAL 700 // Default 544us (Typical ESC-range: 700us-2000us)
#define MAX_SIGNAL 2000  // Default 2400us
#define MOTOR1_PIN 3
#define MOTOR2_PIN 5
#define MOTOR3_PIN 6
#define MOTOR4_PIN 9
#define BAUDRATE 9600


Motor motor1 = {MOTOR1_PIN, "Motor 1"};
Motor motor2 = {MOTOR2_PIN, "Motor 2"};
Motor motor3 = {MOTOR3_PIN, "Motor 3"};
Motor motor4 = {MOTOR4_PIN, "Motor 4"};
Motor motors[4] = {motor1, motor2, motor3, motor4};

int speed = 0;



void setup()
{
  Serial.begin(BAUDRATE);
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
  if (motor.servo.attached() == true)
  {
    motor.servo.detach();
    Serial.print(motor.description);
    Serial.println(" detached");
  }
}

String readSerial()
{
  // Reads the incoming string from the serial. Because software serial is relatively slow, this function
  // attempts to wait (delay) until the entire message is received, ending in end of line ('\n').

  // This function stops reading when it either receives '\n' or the maxDelay is exceeded.
  // Delays are in microseconds   (1000uS = 1ms;    1 000 000uS = 1s)
  unsigned int maxDelay = 50000; // 50ms 
  unsigned int minDelay = 100; // 0.1ms
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
  
  //Serial.print("Delay: ");
  //Serial.println(timeDelayed);
  return input;
}

void parseThrottle(String input)
{
  
  if (input.length() > 3)
  {
    Serial.println("INPUT ERROR: Maximum of 3 digits allowed (0-180)");
    return;
  }
  for(int i = 0; i < input.length(); i++)
  {
    if (input[i] < '0' || input[i] > '9')
    {
      Serial.println("INPUT ERROR: Not a valid number");
      return;
    }
  }
  int value = input.toInt();
  if (value < 0 || value > 180)
  {
    Serial.println("INPUT ERROR: Value is not in range 0-180");
    return;
  }
  Serial.print("Changing throttle to ");
  Serial.println(value);
  speed = value;
}

void parseCommands(String input)
{
  // Attach
  if (input[0] == 'a')
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
        count++;
      }
    }
    Serial.print(count);
    Serial.println(" motors attached");
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
        count++;
      }
    }
    Serial.print(count);
    Serial.println(" motors detached");
  }
  else
  {
    Serial.println("Invalid command. Attach motors with 'a' and detach with 'd'");
    Serial.println("Example: 'a24' - attaches motors 2 and 4");
  }
}

int isMaybeNumber(String input)
{
  if (input[0] < '0' || input[0] > '9')
  {
    return 0;
  }
  return 1;
}

void loop()
{
  // If there is incoming value
  if (Serial.available() > 0)
  {
    String command = readSerial(); // Using String instead of char[] for added functionality (less memory effective)
    command.toLowerCase();
    command.trim();
    Serial.print("received: ");
    Serial.print(command);
    Serial.print('\n');
    if (isMaybeNumber(command) == 1)
    {
      parseThrottle(command);
      for (unsigned int i = 0; i < 4; i++)
      {
        setSpeed(motors[i], speed);
      }
    }
    else
    {
      Serial.println("Attempting to parse a command...");
      parseCommands(command);
    }
    
    
  }
}




