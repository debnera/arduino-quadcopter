#include <SoftwareSerial.h>
#define BDRATE 38400
#define DELAY 100 //ms
SoftwareSerial softSerial(10,11); // RX, TX

String incoming;
char c;
unsigned long prev_time;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(BDRATE);
  softSerial.begin(BDRATE);
  doublePrint("Hello");
  doubleNL();
  prev_time = millis();
}

void doublePrint(String str)
{
  Serial.print(str);
  delay(100); // Delay to avoid interference
  softSerial.print(str);
}

void doubleNL()
{
  Serial.print('\n');
  delay(100); // Delay to avoid interference
  softSerial.print('\n');
}

void loop() {
  // put your main code here, to run repeatedly:
  incoming = "";
  if (softSerial.available())
  {
    doubleNL();
    doublePrint("(sw)Quad says: ");
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
    doublePrint("(hw)Quad says: ");
    delay(DELAY);
    while(Serial.available())
    {
      c = Serial.read();
      incoming += c;
    }
    doublePrint(incoming);
  }
  if ((millis() - prev_time) > 1000)
  {
    softSerial.write("PING\n");
    prev_time = millis();
  }
}
