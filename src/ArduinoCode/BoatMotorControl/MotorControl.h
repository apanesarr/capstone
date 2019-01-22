#ifndef MotorControl_h
#define MotorControl_h

#include "Arduino.h"

#define LEFT_FWD_PIN 5
#define LEFT_BWD_PIN 6
#define RIGHT_FWD_PIN 9
#define RIGHT_BWD_PIN 10

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
  int outputPwr      = 0;
  int secondaryPwr   = 0;
};

class MotorControl
{
  public:
    MotorControl();
    void setMotor(motorState_e newState,
                  float percentOutput   = INVALID_SETTING,
                  float turnSensitivity = INVALID_SETTING);
  private:
    motorSettings_t settings;
    void updateMotorCtrlPins();
    void forward();
    void reverse();
    void steerLeft();
    void steerRight();
    void stop();
};

#endif
