#ifndef MotorControl_h
#define MotorControl_h

#include "Arduino.h"

#define LEFT_FWD_PIN 9
#define LEFT_BWD_PIN 10
#define RIGHT_FWD_PIN 6
#define RIGHT_BWD_PIN 5

#define LEFT_MOTOR_PWR 90
#define RIGHT_MOTOR_PWR LEFT_MOTOR_PWR + 15

#define INVALID_SETTING -1

enum motorState_e {
  FORWARD,
  REVERSE,
  STEER_LEFT,
  STEER_RIGHT,
  STOP
};

struct motorSettings_t {
  motorState_e state = STOP;
};

class MotorControl
{
  public:
    MotorControl();
    void setMotor(motorState_e newState);
  private:
    motorState_e state;
    void updateMotorCtrlPins();
    void forward();
    void reverse();
    void steerLeft();
    void steerRight();
    void stop();
};

#endif
