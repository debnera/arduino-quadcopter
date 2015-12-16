#include "mpu.h"

MPU mpu;

void dmpDataReady() {
    mpu.mpuInterrupt = true;
}

void setup() {
  // put your setup code here, to run once:
	Serial.begin(115200);
	mpu = MPU();
	bool success = mpu.init();
	if(success) attachInterrupt(0, dmpDataReady, RISING);
}

void loop() {
  // put your main code here, to run repeatedly:

}
