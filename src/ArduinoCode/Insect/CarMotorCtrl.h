#ifndef MotorControl_h
#define MotorControl_h

#include "MotorPatterns.h"
#include "Arduino.h"
#include "IMU.h"

#define LEFT_FWD_PIN 6
#define LEFT_BWD_PIN 5
#define RIGHT_FWD_PIN 9
#define RIGHT_BWD_PIN 10

/* Encoder pins - on Uno, interrupt pins are 2, 3.
 * These should be set to encoder output A for each side. */
#define RS_ENC_A 2
#define RS_ENC_B 1
#define LS_ENC_A 3
#define LS_ENC_B 4

#define LEFT_MOTOR_PWR 255
#define RIGHT_MOTOR_PWR LEFT_MOTOR_PWR

#define TRUE 1
#define FALSE 0

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

struct encoder_t {
    long int tickRS;
    long int tickLS;
    float distance;
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
    encoder_t encoder;
    void forward();
    void reverse();
    void left();
    void right();
    void stop();
    void initEncoderPins();
    void updateEncLS();
    void updateEncRS();
    void updateDistance();
};

#endif
