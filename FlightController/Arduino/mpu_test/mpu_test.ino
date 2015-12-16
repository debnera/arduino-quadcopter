#include "mpu.h"
#include "angles.h"

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
  if (mpu.dataAvailable())
  {
    Angles ypr = mpu.getAngles();
    Serial.print("ypr\t");
    Serial.print(ypr.yaw);
    Serial.print("\t");
    Serial.print(ypr.pitch);
    Serial.print("\t");
    Serial.println(ypr.roll);
  }
	
}
