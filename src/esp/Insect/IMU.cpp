#include "IMU.h"
#include "Arduino.h"
#include "EEPROM.h"

void IMU::init() {

    Serial.begin(115200);
    while(!Serial) {}

    /* Start communication with IMU */
    status = sensor.begin(D2, D3);
    if (status < 0) {
        Serial.println("IMU initialization unsuccessful");
        Serial.println("Check IMU wiring or try cycling power");
        Serial.print("Status: ");
        Serial.println(status);
        //while(1) {}
    }
    /* Setting the accelerometer full scale range to +/-8G */
    sensor.setAccelRange(MPU9250::ACCEL_RANGE_8G);

    /* Setting the gyroscope full scale range to +/-500 deg/s */
    sensor.setGyroRange(MPU9250::GYRO_RANGE_500DPS);

    /* Setting DLPF bandwidth to 20 Hz */
    sensor.setDlpfBandwidth(MPU9250::DLPF_BANDWIDTH_20HZ);

    /* Setting SRD to 19 for a 50 Hz update rate */
    sensor.setSrd(19);

    /* Get mag calibration data from EEPROM */
    EEPROM.get(MAG_CAL_DATA_EEPROM_ADDR, magCalParams);
    sensor.setMagCalX(magCalParams.xBias, magCalParams.xScaleFactor);
    sensor.setMagCalY(magCalParams.yBias, magCalParams.yScaleFactor);
    sensor.setMagCalZ(magCalParams.zBias, magCalParams.zScaleFactor);

    yaw = 0;
}

float IMU::getTemp() {
    return sensor.getTemperature_C();
};

void IMU::calibrateMag() {

    /* Ensure indicator LED is off */
    pinMode(LED_BUILTIN, OUTPUT);
    digitalWrite(LED_BUILTIN, LOW);

    /**
     * Run the calibrate function from the library. According to the library,
     * "the sensor should be rotated in a figure 8 motion until complete."
     */
    Serial.print("Calibrating...");
    sensor.calibrateMag();
    Serial.println("Done.");

    /* Bias */
    magCalParams.xBias = sensor.getMagBiasX_uT();
    magCalParams.yBias = sensor.getMagBiasY_uT();
    magCalParams.zBias = sensor.getMagBiasZ_uT();

    /* Scale factor */
    magCalParams.xScaleFactor = sensor.getMagScaleFactorX();
    magCalParams.yScaleFactor = sensor.getMagScaleFactorY();
    magCalParams.zScaleFactor = sensor.getMagScaleFactorZ();

    /* Write to EEPROM */
    Serial.print("Writing calibration data to EEPROM...");
    EEPROM.put(MAG_CAL_DATA_EEPROM_ADDR, magCalParams);
    Serial.println("Done.");
    Serial.println("Calibration complete.");

    /* Indicate cal completed */
    digitalWrite(LED_BUILTIN, HIGH);
}

int IMU::update()
{
    if (sensor.readSensor() < 0) {
        return -1;
    }
    dt = millis() - dt;
    float gyroZ = -sensor.getGyroZ_rads();
    yaw += gyroZ * dt / 1000;
    dt = millis();
    return 1;
}

int IMU::getYaw()
{
    int angle = (int) (round(yaw * 180 / M_PI) + 360) % 360;
    Serial.println(angle);
    return angle;
}

