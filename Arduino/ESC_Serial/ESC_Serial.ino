#include <Servo.h>

#define MIN_SIGNAL 700 // Default 544us (Typical ESC-range: 700us-2000us)
#define MAX_SIGNAL 2000  // Default 2400us
#define MOTOR_PIN 3

Servo myMotor;
int speed = 0;
String incomingString;


void setup()
{
  Serial.begin(9600);
  speed = 0; // Set default speed
  Serial.println("Starting speed set to 0");
  Serial.println("Motor offline, waiting for input - Attach with 'a', detach with 'd'");
}

void setSpeed(int angle) {
  myMotor.write(angle);
}

void attachMotor()
{
  myMotor.attach(MOTOR_PIN, MIN_SIGNAL, MAX_SIGNAL); // Attach pin, set min and max pwm range
  Serial.println("Motor attached");
  String a = "PWM range is set to ";
  a += MIN_SIGNAL;
  a += "uS - ";
  a += MAX_SIGNAL;
  a += "uS";
  Serial.println(a);
  myMotor.write(speed);
}

void detachMotor()
{
  myMotor.detach();
  Serial.println("Motor detached");
}

void loop()
{
  // If there is incoming value
  if (Serial.available() > 0)
  {
    // read the value
    char ch = Serial.read();
    if (ch == 'a' && Serial.peek() == '\n')
    {
      attachMotor();
      incomingString = "";
    }
    if (ch == 'd' && Serial.peek() == '\n')
    {
      detachMotor();
      incomingString = "";
    }
    if (ch != 10) { // 10 equals to newline
      Serial.print("I have received: ");
      Serial.print(ch, DEC);
      Serial.print('\n');
      incomingString += ch;
    }
    else if (incomingString.length() > 0) 
    {
      // print the incoming string
      Serial.println("I am printing the entire string");
      Serial.println(incomingString);

      // Convert the string to an integer
      int val = incomingString.toInt();

      // print the integer
      Serial.println("Printing the value: ");
      Serial.println(val);

      /*
      *  We only want to write an integer between
      *  0 and 180 to the motor.
      */
      if (val > -1 && val < 181)
      {
        Serial.println("Value is between 0 and 180");
        // Set the speed and write it to the motor, if motor is attached.
        speed = val;
        if (myMotor.attached() == true)
        {
          myMotor.write(val);
        }
        else
        {
          Serial.println("Motor is not attached, attach by sending 'a' - starting speed changed to given value");
        }
      }
      else
      {
        Serial.println("Value is NOT between 0 and 180");
        Serial.println("Error with the input");
      }

      // Reset the value of the incomingString
      incomingString = "";
    }
  }
}
