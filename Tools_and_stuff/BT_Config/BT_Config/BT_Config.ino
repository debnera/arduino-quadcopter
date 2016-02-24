/*
 * Used to configure HC-05.
 * Connect KEY pin to 5v/3.3v or pin 9 to activate AT-mode.
 *
 * Commands:
 * AT (check connection)
 * AT+NAME (check name)
 * AT+UART (check baudrate)
 * AT+UART=115200 (set baudrate to 115200)
 * AT+RESET (save changes and exit AT-mode)
 */


#include <SoftwareSerial.h>

SoftwareSerial mySerial(7, 8); // RX, TX

void setup() {
  Serial.begin(9600);
  pinMode(9, OUTPUT); digitalWrite(9, HIGH);
  Serial.println("Send 'AT' now -  The device should respond with 'OK' if connection is ok");
  mySerial.begin(38400);
}

void loop() {
  if (mySerial.available())
    Serial.write(mySerial.read());
  if (Serial.available())
    mySerial.write(Serial.read());
}
