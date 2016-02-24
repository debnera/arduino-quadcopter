// I2C device class (I2Cdev) demonstration Arduino sketch for MPU6050 class
// 10/7/2011 by Jeff Rowberg <jeff@rowberg.net>
// Updates should (hopefully) always be available at https://github.com/jrowberg/i2cdevlib
//
// Changelog:
//      2013-05-08 - added multiple output formats
//                 - added seamless Fastwire support
//      2011-10-07 - initial release

/* ============================================
I2Cdev device library code is placed under the MIT license
Copyright (c) 2011 Jeff Rowberg

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
===============================================
*/

// I2Cdev and MPU6050 must be installed as libraries, or else the .cpp/.h files
// for both classes must be in the include path of your project
#include "I2Cdev.h"
#include "MPU6050.h"

// Arduino Wire library is required if I2Cdev I2CDEV_ARDUINO_WIRE implementation
// is used in I2Cdev.h
#if I2CDEV_IMPLEMENTATION == I2CDEV_ARDUINO_WIRE
    #include "Wire.h"
#endif

// class default I2C address is 0x68
// specific I2C addresses may be passed as a parameter here
// AD0 low = 0x68 (default for InvenSense evaluation board)
// AD0 high = 0x69
MPU6050 accelgyro;
//MPU6050 accelgyro(0x69); // <-- use for AD0 high

int16_t ax, ay, az;
int16_t gx, gy, gz;



// uncomment "OUTPUT_READABLE_ACCELGYRO" if you want to see a tab-separated
// list of the accel X/Y/Z and then gyro X/Y/Z values in decimal. Easy to read,
// not so easy to parse, and slow(er) over UART.
#define OUTPUT_READABLE_ACCELGYRO

// uncomment "OUTPUT_BINARY_ACCELGYRO" to send all 6 axes of data as 16-bit
// binary, one right after the other. This is very fast (as fast as possible
// without compression or data loss), and easy to parse, but impossible to read
// for a human.
//#define OUTPUT_BINARY_ACCELGYRO


#define LED_PIN 13
bool blinkState = false;
int countBefore = 0;
int countAfter = 1000;
int count = 0;
int32_t Tax, Tay, Taz;
int32_t Tgx, Tgy, Tgz;

int Offax, Offay, Offaz;
int Offgx, Offgy, Offgz;

int calc = 0;

void setup() {
    // join I2C bus (I2Cdev library doesn't do this automatically)
    #if I2CDEV_IMPLEMENTATION == I2CDEV_ARDUINO_WIRE
        Wire.begin();
    #elif I2CDEV_IMPLEMENTATION == I2CDEV_BUILTIN_FASTWIRE
        Fastwire::setup(400, true);
    #endif

    // initialize serial communication
    // (38400 chosen because it works as well at 8MHz as it does at 16MHz, but
    // it's really up to you depending on your project)
    Serial.begin(115200);

    // initialize device
    Serial.println("Initializing I2C devices...");
    accelgyro.initialize();

    // verify connection
    Serial.println("Testing device connections...");
    Serial.println(accelgyro.testConnection() ? "MPU6050 connection successful" : "MPU6050 connection failed");

    // use the code below to change accel/gyro offset values

    Serial.println("Updating internal sensor offsets...");
    accelgyro.setXAccelOffset(-3114);
    accelgyro.setYAccelOffset(-828);
    accelgyro.setZAccelOffset(2031);
    accelgyro.setXGyroOffset(78);
    accelgyro.setYGyroOffset(-42);
    accelgyro.setZGyroOffset(48);
    // -3114	-828	2031	78	-42	48


    
    Serial.print(accelgyro.getXAccelOffset()); Serial.print("\t"); // -76
    Serial.print(accelgyro.getYAccelOffset()); Serial.print("\t"); // -2359
    Serial.print(accelgyro.getZAccelOffset()); Serial.print("\t"); // 1688
    Serial.print(accelgyro.getXGyroOffset()); Serial.print("\t"); // 0
    Serial.print(accelgyro.getYGyroOffset()); Serial.print("\t"); // 0
    Serial.print(accelgyro.getZGyroOffset()); Serial.print("\t"); // 0
    Serial.print("\n");
    delay(3000);
    Offax = accelgyro.getXAccelOffset(); 
    Offay = accelgyro.getYAccelOffset(); 
    Offaz = accelgyro.getZAccelOffset(); 
    Offgx = accelgyro.getXGyroOffset(); 
    Offgy = accelgyro.getYGyroOffset(); 
    Offgz = accelgyro.getZGyroOffset(); 


    // configure Arduino LED for
    pinMode(LED_PIN, OUTPUT);
}

int adjust(int offset, int sensorValue, int targetValue)
{
    int fraction = (sensorValue-targetValue)/1000;
    if(sensorValue < targetValue - 2)
    {
      return (offset + abs(fraction) + 1);
    }
    else if(sensorValue > targetValue + 2)
    {
      return (offset - abs(fraction) - 1);
    }
    else return offset;
}

void loop() {
    // read raw accel/gyro measurements from device
    accelgyro.getMotion6(&ax, &ay, &az, &gx, &gy, &gz);

    // these methods (and a few others) are also available
    //accelgyro.getAcceleration(&ax, &ay, &az);
    //accelgyro.getRotation(&gx, &gy, &gz);
    count = count + 1;
    if(count < countAfter)
    {
        Tax = Tax + ax;
        Tay = Tay + ay;
        Taz = Taz + az;
        Tgx = Tgx + gx;
        Tgy = Tgy + gy;
        Tgz = Tgz + gz;
    }
    else if(count == countAfter)
    {
        Tax = Tax / count;
        Tay = Tay / count;
        Taz = Taz / count;
        Tgx = Tgx / count;
        Tgy = Tgy / count;
        Tgz = Tgz / count;
        Serial.print("Calculation complete: ");
        Serial.println(calc);
        calc = calc + 1;
        
        Serial.print("F_values:\t");
        Serial.print(ax); Serial.print("\t");
        Serial.print(ay); Serial.print("\t");
        Serial.print(az); Serial.print("\t");
        Serial.print(gx); Serial.print("\t");
        Serial.print(gy); Serial.print("\t");
        Serial.println(gz);
        
        Serial.print("A_values:\t");
        Serial.print(Tax); Serial.print("\t");
        Serial.print(Tay); Serial.print("\t");
        Serial.print(Taz); Serial.print("\t");
        Serial.print(Tgx); Serial.print("\t");
        Serial.print(Tgy); Serial.print("\t");
        Serial.println(Tgz);
        count = 0;
        
        Offax = adjust(Offax, Tax, 16384);
        Offay = adjust(Offay, Tay, 0);
        Offaz = adjust(Offaz, Taz, 0);
        
        Offgx = adjust(Offgx, Tgx, 0);
        Offgy = adjust(Offgy, Tgy, 0);
        Offgz = adjust(Offgz, Tgz, 0);
        
        accelgyro.setXAccelOffset(Offax);
        accelgyro.setYAccelOffset(Offay);
        accelgyro.setZAccelOffset(Offaz);
        accelgyro.setXGyroOffset(Offgx);
        accelgyro.setYGyroOffset(Offgy);
        accelgyro.setZGyroOffset(Offgz);
        
        Serial.print("Accuracy: ");
        Serial.println(countAfter);
        
        
        Serial.print("Read offset :\t");
        Serial.print(accelgyro.getXAccelOffset()); Serial.print("\t"); // -76
        Serial.print(accelgyro.getYAccelOffset()); Serial.print("\t"); // -2359
        Serial.print(accelgyro.getZAccelOffset()); Serial.print("\t"); // 1688
        Serial.print(accelgyro.getXGyroOffset()); Serial.print("\t"); // 0
        Serial.print(accelgyro.getYGyroOffset()); Serial.print("\t"); // 0
        Serial.print(accelgyro.getZGyroOffset()); Serial.print("\t"); // 0
        Serial.print("\n");
        
        Tax = 0;
        Tay = 0;
        Taz = 0;
        Tgx = 0;
        Tgy = 0;
        Tgz = 0;
    }

    // blink LED to indicate activity
    blinkState = !blinkState;
    digitalWrite(LED_PIN, blinkState);
}
