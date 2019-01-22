#ifndef MotorControl_h
#define MotorControl_h

#include "Arduino.h"

#define LEFT_PWR_PIN 9
#define RIGHT_PWR_PIN 10
#define LEFT_FWD_PIN 3
#define LEFT_BWD_PIN 4
#define RIGHT_FWD_PIN 5
#define RIGHT_BWD_PIN 6

enum motorState_e {
  FORWARD,
  REVERSE,
  STEERLEFT,
  STEERRIGHT,
  STOPPED
}

struct motorState_t {
  motorState_e state;
  int outputPwr;
  int ratio;
}

class MotorControl
{
  public:
    MotorControl();
    void setMotorState(motorState_t newState);

  private:
    motorState_t motorState;
    void updateMotorState();
    void forward();
    void reverse();
    void steerLeft();
    void steerRight();
    void stopped();
};

#endif
