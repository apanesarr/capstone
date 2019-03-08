#ifndef MotorControl_h
#define MotorControl_h

#include "MotorPatterns.h"
#include "Arduino.h"
#include "IMU.h"
#include <limits.h>

#define LEFT_FWD_PIN 6
#define LEFT_BWD_PIN 5
#define RIGHT_FWD_PIN 9
#define RIGHT_BWD_PIN 10

#define LEFT_MOTOR_PWR 255
#define RIGHT_MOTOR_PWR LEFT_MOTOR_PWR

#define TRUE 1
#define FALSE 0

#define INDEFINITE LONG_MAX

enum motorState_e {
  FORWARD,
  REVERSE,
  LEFT,
  RIGHT,
  STOP
};

struct motorSettings_t {
  motorState_e state;
  float targetAngle;
  long int targetTimeMs;
};

class MotorControl
{
  public:
    MotorControl();
    void setMotor(motorSettings_t newSettings);
    int update();
    void init();
    bool ready;
  private:
    IMU imu;
    motorSettings_t settings;
    void forward();
    void reverse();
    void left();
    void right();
    void stop();
    void resetPinState();
    bool commsError;
};

#endif
