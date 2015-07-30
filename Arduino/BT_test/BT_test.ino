#include <SoftwareSerial.h>
#define BDRATE 9600
#define DELAY 1000 //ms
SoftwareSerial softSerial(10,11); // RX, TX

String incoming;
char c;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(BDRATE);
  softSerial.begin(BDRATE);
  doublePrint("Hello");
  doubleNL();
}

void doublePrint(String str)
{
  Serial.print(str);
  softSerial.print(str);
}

void doubleNL()
{
  Serial.print('\n');
  softSerial.print('\n');
}

void loop() {
  // put your main code here, to run repeatedly:
  incoming = "";
  if (softSerial.available())
  {
    doubleNL();
    doublePrint("SW Received: ");
    delay(DELAY);
    while(softSerial.available())
    {
      c = softSerial.read();
      incoming += c;
      
    }
    doublePrint(incoming);
  }
    
  if (Serial.available())
  {
    doubleNL();
    doublePrint("HW Received: ");
    delay(DELAY);
    while(Serial.available())
    {
      c = Serial.read();
      incoming += c;
    }
    doublePrint(incoming);
  }
    
}
