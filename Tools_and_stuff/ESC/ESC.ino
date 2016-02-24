
#include <Servo.h> 

Servo myservo;
int armDelay = 5000;
int maxSpeed = 70;

void arm(){  
  Serial.print('Arming now'); 
  setSpeed(0);
  delay(2000); //delay 1 second,  some speed controllers may need longer
  Serial.print('Armed');
}

void setSpeed(int angle){
  // speed is from 0 to 100 where 0 is off and 100 is maximum speed
  //the following maps speed values of 0-100 to angles from 0-180,
  // some speed controllers may need different values, see the ESC instructions
  myservo.write(angle);    
  Serial.println(angle); 
}

void setup()
{
  Serial.begin(9600);
  Serial.print('Arming in ');
  Serial.println(armDelay);
  delay(armDelay);
  myservo.attach(3);
  arm();  
}


void loop()
{
  int speed;

  // sweep up from 0 to to maximum speed
  for(speed = 0; speed <= maxSpeed; speed += 1) {
    setSpeed(speed);
    delay(100);
  }
  // sweep back down to 0 speed.
  for(speed = maxSpeed; speed > 0; speed -= 1) {
    setSpeed(speed);
    delay(100);
  }
  setSpeed(0);  
  delay(500000);
}
