#ifndef IMU_h
#define IMU_h

#include "MPU9250.h"

#define I2C_ADDR_IMU             0x68
#define MAG_CAL_DATA_EEPROM_ADDR 0x00

struct magCalParams_t {
    float xBias;
    float yBias;
    float zBias;
    float xScaleFactor = 1.0f;
    float yScaleFactor = 1.0f;
    float zScaleFactor = 1.0f;
};

class IMU {
    public:
        IMU() : sensor(Wire, I2C_ADDR_IMU) {};
        void init();
        float getTemp();
        int update();
        int getYaw();
        void calibrateMag();
    private:
        MPU9250 sensor;
        magCalParams_t magCalParams;
        int status;
        double roll, pitch, yaw;
        long int dt = 0;
};

#endif
