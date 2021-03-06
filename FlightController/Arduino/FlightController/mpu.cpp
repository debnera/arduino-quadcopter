/*
Created:	9/07/2015
Author:		Anton
*/
/*
This is mostly adapted from the mpu6050 example from http://www.i2cdevlib.com/
*/

#include "mpu.h"

MPU::MPU(/*SerialCommunicator *serial_communicator*/)
{
  //serCom = serial_communicator;
}


bool MPU::init()
{
	#if I2CDEV_IMPLEMENTATION == I2CDEV_ARDUINO_WIRE
		Wire.begin();
		TWBR = 24; // 400kHz I2C clock (200kHz if CPU is 8MHz)
	#elif I2CDEV_IMPLEMENTATION == I2CDEV_BUILTIN_FASTWIRE
		Fastwire::setup(400, true);
	#endif

	// initialize device
    DEBUG_PRINTLN(F("Initializing I2C devices..."));
    mpu.initialize();

    // verify connection
    DEBUG_PRINTLN(F("Testing device connections..."));
    DEBUG_PRINTLN(mpu.testConnection() ? F("MPU6050 connection successful") : F("MPU6050 connection failed"));
	if (mpu.testConnection() == false)
	{
		return false;
	}
    // wait for ready
    DEBUG_PRINTLN(F("\nSend any character to begin DMP programming and demo: "));
    while (Serial.available() && Serial.read()); // empty buffer
    // while (!Serial.available());                 // wait for data
    // while (Serial.available() && Serial.read()); // empty buffer again

    // load and configure the DMP
    DEBUG_PRINTLN(F("Initializing DMP..."));
    devStatus = mpu.dmpInitialize();
		gyro_sensitivity = mpu.getFullScaleGyroRange(); // Default should be 2000
		DEBUG_PRINT(F("Gyro sensitivity set to "));
		DEBUG_PRINTLN(gyro_sensitivity);
    // supply your own gyro offsets here, scaled for min sensitivity
    mpu.setXGyroOffset(80);
    mpu.setYGyroOffset(-50);
    mpu.setZGyroOffset(50);
		mpu.setXAccelOffset(-3075);
		mpu.setYAccelOffset(-920);
    mpu.setZAccelOffset(2095); // 1688 factory default for my test chip

    // make sure it worked (returns 0 if so)
    if (devStatus == 0) {
        // turn on the DMP, now that it's ready
        DEBUG_PRINTLN(F("Enabling DMP..."));
        mpu.setDMPEnabled(true);

        // enable Arduino interrupt detection
        DEBUG_PRINTLN(F("Enabling interrupt detection (Arduino external interrupt 0)..."));
		mpuInterrupt = false;

		/*
		NOTE:   INTERRUPT MUST BE ATTACHED IN THE MAIN .ino FILE
		This is a hackish way around the limitation of not being able to attachInterrupt
		to a class member.

		void dmpDataReady() {
		_INSTANCE_OF_THIS_CLASS_.mpuInterrupt = true;
		}

		attachInterrupt(0, dmpDataReady, RISING);
		*/

        mpuIntStatus = mpu.getIntStatus();

        // set our DMP Ready flag so the main loop() function knows it's okay to use it
        DEBUG_PRINTLN(F("DMP ready! Waiting for first interrupt..."));
        dmpReady = true;

        // get expected DMP packet size for later comparison
        packetSize = mpu.dmpGetFIFOPacketSize();
    } else {
        // ERROR!
        // 1 = initial memory load failed
        // 2 = DMP configuration updates failed
        // (if it's going to break, usually the code will be 1)
        DEBUG_PRINT(F("DMP Initialization failed (code "));
        DEBUG_PRINT(devStatus);
        DEBUG_PRINTLN(F(")"));
		return false;
    }
	return true;
}

Angles MPU::getAngles()
{
	Angles orientation = Angles(0,0,0);
	if (!dmpReady) return orientation;

	// get current FIFO count
	mpuInterrupt = false;
    mpuIntStatus = mpu.getIntStatus();
    fifoCount = mpu.getFIFOCount();

    // check for overflow (this should never happen unless our code is too inefficient)
    if ((mpuIntStatus & 0x10) || fifoCount == 1024) {
        // reset so we can continue cleanly
        mpu.resetFIFO();
        DEBUG_PRINTLN(F("FIFO overflow!"));

    // otherwise, check for DMP data ready interrupt (this should happen frequently)
    }
	else if (mpuIntStatus & 0x02) {
        // wait for correct available data length, should be a VERY short wait
        while (fifoCount < packetSize) fifoCount = mpu.getFIFOCount();

        // read a packet from FIFO
        mpu.getFIFOBytes(fifoBuffer, packetSize);

        // track FIFO count here in case there is > 1 packet available
        // (this lets us immediately read more without waiting for an interrupt)
        fifoCount -= packetSize;


		mpu.dmpGetQuaternion(&q, fifoBuffer);
		mpu.dmpGetGravity(&gravity, &q);
		mpu.dmpGetYawPitchRoll(ypr, &q, &gravity);
		orientation = Angles(ypr[0] * 180/M_PI, ypr[1] * 180/M_PI, ypr[2] * 180/M_PI);

	}
	else{
		Serial.println("FAIL");
		//mpu.getFIFOBytes(fifoBuffer, packetSize);
		mpu.resetFIFO();
	}

	return orientation;

}

Angles MPU::getAngularRates()
{
	//http://www.i2cdevlib.com/forums/topic/106-get-angular-velocity-from-mpu-6050/
	mpu.dmpGetGyro(&gyro, fifoBuffer);
	int16_t max_size = 32767; //pow(2,15) is too large for 16 bit int
	Angles rates = Angles(gyro.x, gyro.y, gyro.z) * gyro_sensitivity;
	rates = rates / max_size;
	return rates;
}

bool MPU::fifoOverflow()
{
	// Returns true if fifo has overflown. False if everything is ok.
	return (mpu.getFIFOCount() == 1024);
}

bool MPU::dataAvailable()
{
	// Returns true if there is any data to be received
	return (dmpReady && (mpuInterrupt || fifoCount >= packetSize));
}

void MPU::setGyroScale(int sensitivity)
{
	if (sensitivity == 250)
	{

		mpu.setFullScaleGyroRange(MPU6050_GYRO_FS_250);
	}
	else if (sensitivity == 500)
	{
		mpu.setFullScaleGyroRange(MPU6050_GYRO_FS_500);
	}
	else if (sensitivity == 1000)
	{
		mpu.setFullScaleGyroRange(MPU6050_GYRO_FS_1000);
	}
	else if (sensitivity == 2000)
	{
		mpu.setFullScaleGyroRange(MPU6050_GYRO_FS_2000);
	}
	else{
		DEBUG_PRINTLN("Invalid gyro range");
		return; // Don't change gyro_sensitivity variable
	}
	gyro_sensitivity = sensitivity;
}
