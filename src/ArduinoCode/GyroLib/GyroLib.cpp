#include "I2Cdev.h"
#include "MPU6050.h"
#include "CarMotorCtrl.h"
#include "Arduino.h"
#include "GyroLib.h"
#include "Wire.h"
/*
Gyro::Gyro() : filter(25, 0) {

    #if I2CDEV_IMPLEMENTATION == I2CDEV_ARDUINO_WIRE
        Wire.begin();
    #elif I2CDEV_IMPLEMENTATION == I2CDEV_BUILTIN_FASTWIRE
        Fastwire::setup(400, true);
    #endif
    Serial.begin(38400);

    // initialize device
    Serial.println("Initializing I2C devices...");
    accelgyro.initialize();

    // verify connection
    Serial.println("Testing device connections...");
    Serial.println(accelgyro.testConnection() ? "MPU6050 connection successful" : "MPU6050 connection failed");

    delay(100);
    Serial.println("Updating internal sensor offsets...");

    accelgyro.setXGyroOffset(0);
    accelgyro.setYGyroOffset(0);
    accelgyro.setZGyroOffset(0);
    delay(10);
    for (int i=0; i<25; i++) {
      accelgyro.getRotation(&gx, &gy, &gz);
      gxx[i] = gx;
      gyy[i] = gy;
      gzz[i] = gz;
      Serial.println(gx);
    }

    for (int i=0; i<25; i++){;
      avgx += gxx[i]/25;
      avgy += gyy[i]/25;
      avgz += gzz[i]/25;
    }
    // -76	-2359	1688	0	0	0
    Serial.print(accelgyro.getXAccelOffset()); Serial.print("\t"); // -76
    Serial.print(accelgyro.getYAccelOffset()); Serial.print("\t"); // -2359
    Serial.print(accelgyro.getZAccelOffset()); Serial.print("\t"); // 1688
    Serial.print(accelgyro.getXGyroOffset()); Serial.print("\t"); // 0
    Serial.print(accelgyro.getYGyroOffset()); Serial.print("\t"); // 0
    Serial.print(accelgyro.getZGyroOffset()); Serial.print("\t"); // 0
    Serial.print("\n");
    Serial.println(avgx);
    Serial.print(accelgyro.getXAccelOffset()); Serial.print("\t"); // -76
    Serial.print(accelgyro.getYAccelOffset()); Serial.print("\t"); // -2359
    Serial.print(accelgyro.getZAccelOffset()); Serial.print("\t"); // 1688
    Serial.print(accelgyro.getXGyroOffset()); Serial.print("\t"); // 0
    Serial.print(accelgyro.getYGyroOffset()); Serial.print("\t"); // 0
    Serial.print(accelgyro.getZGyroOffset()); Serial.print("\t"); // 0
    Serial.print("\n");
}
*/
void Gyro::init() {

    #if I2CDEV_IMPLEMENTATION == I2CDEV_ARDUINO_WIRE
        Wire.begin();
    #elif I2CDEV_IMPLEMENTATION == I2CDEV_BUILTIN_FASTWIRE
        Fastwire::setup(400, true);
    #endif
    Serial.begin(38400);

    // initialize device
    Serial.println("Initializing I2C devices...");
    accelgyro.initialize();

    // verify connection
    Serial.println("Testing device connections...");
    Serial.println(accelgyro.testConnection() ? "MPU6050 connection successful" : "MPU6050 connection failed");

    delay(100);
    Serial.println("Updating internal sensor offsets...");

    accelgyro.setXGyroOffset(0);
    accelgyro.setYGyroOffset(0);
    accelgyro.setZGyroOffset(0);
    delay(10);
    for (int i=0; i<25; i++) {
      accelgyro.getRotation(&gx, &gy, &gz);
      gxx[i] = gx;
      gyy[i] = gy;
      gzz[i] = gz;
      Serial.println(gx);
    }

    for (int i=0; i<25; i++){;
      avgx += gxx[i]/25;
      avgy += gyy[i]/25;
      avgz += gzz[i]/25;
    }
    // -76	-2359	1688	0	0	0
    Serial.print(accelgyro.getXAccelOffset()); Serial.print("\t"); // -76
    Serial.print(accelgyro.getYAccelOffset()); Serial.print("\t"); // -2359
    Serial.print(accelgyro.getZAccelOffset()); Serial.print("\t"); // 1688
    Serial.print(accelgyro.getXGyroOffset()); Serial.print("\t"); // 0
    Serial.print(accelgyro.getYGyroOffset()); Serial.print("\t"); // 0
    Serial.print(accelgyro.getZGyroOffset()); Serial.print("\t"); // 0
    Serial.print("\n");
    Serial.println(avgx);
    Serial.print(accelgyro.getXAccelOffset()); Serial.print("\t"); // -76
    Serial.print(accelgyro.getYAccelOffset()); Serial.print("\t"); // -2359
    Serial.print(accelgyro.getZAccelOffset()); Serial.print("\t"); // 1688
    Serial.print(accelgyro.getXGyroOffset()); Serial.print("\t"); // 0
    Serial.print(accelgyro.getYGyroOffset()); Serial.print("\t"); // 0
    Serial.print(accelgyro.getZGyroOffset()); Serial.print("\t"); // 0
    Serial.print("\n");
}

void Gyro::setAngle(float degrees) {

    if ((180.0 < degrees) && (degrees < 360.0)) {
        degrees = degrees - 360;
    }
    degrees = degrees - prevAngle;
    int startTime;
    if ((angle < degrees)) { //&& (0 < degrees < 180)) {
        motor.setMotor(STEER_LEFT);
        while (angle < degrees) {
            getAngle();
            Serial.print("ANGLE: ");
            Serial.print(angle);
            Serial.print("DEGREES: ");
            Serial.println(degrees);
            Serial.println("Motor Go LEFT");
        }
        motor.setMotor(STOP);
    }
    else if ((angle > degrees)) { //&& (0 < degrees < 180)) {
        motor.setMotor(STEER_RIGHT);
        while (angle > degrees) {
            startTime = millis();
            getAngle();
            Serial.print("ANGLE: ");
            Serial.print(angle);
            Serial.print("DEGREES: ");
            Serial.println(degrees);
            Serial.println("Motor Go RIGHT");
            loopTime = millis() - startTime;
        }
        motor.setMotor(STOP);
    }
    zero();
}

void Gyro::getAngle() {

    // read raw accel/gyro measurements from device
    accelgyro.getMotion6(&ax, &ay, &az, &gx, &gy, &gz);

    // these methods (and a few others) are also available
    //accelgyro.getAcceleration(&ax, &ay, &az);
    //accelgyro.getRotation(&gx, &gy, &gz);
    filter.in(gz-avgz);
    if (abs(filter.out()) > 50){
      angle += filter.out()*0.125/1000;
      if (angle > 360.0 || angle < -360) {
         angle = 0.0;
      }
    }
    int a = tau / (tau + 0.125);
    angle = a * (angle) + (1 - a) * ax;
}

void Gyro::zero() {
    angle = 0;
    accelgyro.setXGyroOffset(0);
    accelgyro.setYGyroOffset(0);
    accelgyro.setZGyroOffset(0);
    delay(10);
    for (int i=0; i<25; i++) {
      accelgyro.getRotation(&gx, &gy, &gz);
      gxx[i] = gx;
      gyy[i] = gy;
      gzz[i] = gz;
      Serial.println(gx);
    }

    for (int i=0; i<25; i++){;
      avgx += gxx[i]/25;
      avgy += gyy[i]/25;
      avgz += gzz[i]/25;
    }
}
