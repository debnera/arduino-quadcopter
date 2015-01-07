#include <Servo.h> 

Servo myservo;
int curSpeed;
int offset = 0;

void setSpeed(int angle){
  myservo.write(angle);    
}

void setup()
{
  Serial.begin(9600);
  myservo.attach(3);
  Serial.println("Motor attached");
  Serial.println("Speed set to -1");
  curSpeed = -1;
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
