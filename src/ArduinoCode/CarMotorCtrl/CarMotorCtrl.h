#ifndef CarMotorCtrl_h
#define CarMotorCtrl_h

#define FWD_PIN 5
#define BWD_PIN 6
#define LEFT_PIN 9
#define RIGHT_PIN 10

#define INVALID_SETTING -1

enum motorState_e {
  FORWARD,
  REVERSE,
  FWD_LEFT,
  FWD_RIGHT,
  BACK_LEFT,
  BACK_RIGHT,
  STOP
};

struct motorSettings_t {
  motorState_e state = STOP;
  int outputPwr      = 0;
};

class MotorControl
{
  public:
    MotorControl();
    void setMotor(motorState_e newState, float percentOutput);
  private:
    motorSettings_t settings;
    void updateMotorCtrlPins();
    void forward();
    void reverse();
    void left();
    void right();
    void resetSteering();
    void stop();
};

#endif
