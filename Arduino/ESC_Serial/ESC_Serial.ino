#include <Servo.h> 

#define MIN_SIGNAL 700 // Default 544us (Typical ESC-range: 700us-2000us)
#define MAX_SIGNAL 2000  // Default 2400us
#define MOTOR_PIN 3

Servo myservo;
int curSpeed;
int offset = 0;


void setup()
{
  Serial.begin(9600);
  myservo.attach(MOTOR_PIN, MIN_SIGNAL, MAX_SIGNAL); // Attach pin, set min and max pwm range
  Serial.println("Motor attached");
  String a = "PWM range is set to ";
  a += MIN_SIGNAL;
  a += "uS - ";
  a += MAX_SIGNAL;
  a += "uS";
  Serial.println(a);
  curSpeed = -1;
}

void setSpeed(int angle){
  myservo.write(angle);    
}


void loop()
{
  if (Serial.available())
  {
    char c = Serial.read();
    if('0' <= c && c <= '9')
    {
      curSpeed = (c - '0')*10 + offset;
      Serial.print("Speed set to ");
      Serial.println(curSpeed);
    }
    else if(c == 'q')
    {
      offset = 0;
      Serial.print("Offset set to ");
      Serial.println(offset);
    }
    else if(c == 'w')
    {
      offset = 90;
      Serial.print("Offset set to ");
      Serial.println(offset);
    }
    else if(c == 's')
    {
      curSpeed = -1;
      Serial.print("Speed set to -1  --> Motor disabled");
      Serial.println(curSpeed);
    }
    else
    {
      Serial.println("Unknown command");
    }
    
  }
  if(0 <= curSpeed && curSpeed >= 180)
  {
    setSpeed(curSpeed);  
  }
}
