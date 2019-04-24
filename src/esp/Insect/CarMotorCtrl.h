#ifndef MotorControl_h
#define MotorControl_h

#include "MotorPatterns.h"
#include <ESP8266WiFi.h>
#include "IMU.h"

// #define DEBUG

#ifdef DEBUG
#define DEBUG_PRINT(label, var) Serial.print(label); \
                                Serial.print("\t");  \
                                Serial.println(var);

#define GRAPH_PRINT(w, x, y, z) Serial.print(millis()); \
                                Serial.print(",");  \
                                Serial.print(w);    \
                                Serial.print(",");  \
                                Serial.print(x);    \
                                Serial.print(",");  \
                                Serial.print(y);    \
                                Serial.print(",");  \
                                Serial.println(z);    \




#else
#define DEBUG_PRINT

#endif

#define LEFT_FWD_PIN 14
#define LEFT_BWD_PIN 15
#define RIGHT_FWD_PIN 12
#define RIGHT_BWD_PIN 2

/* Encoder pins - on Uno, interrupt pins are 2, 3.
 * These should be set to encoder output A for each side. */
#define RS_ENC_A 5
/* Pin 1 doesn't work while printing serial */
//#define RS_ENC_B 1
#define LS_ENC_A 13
//#define LS_ENC_B 4

/* Rover wheel radius in mm */
#define RADIUS_SCALE_FACTOR 1.15
#define WHEEL_RADIUS 31 * RADIUS_SCALE_FACTOR
#define TICKS_PER_REV 166.5

#define MOTOR_PWR 593
#define RIGHT_OFFSET 0.89
/* Rate in ms for compensating between motors */
#define UPDATE_RATE 50
#define P_GAIN 7
#define D_GAIN 3
#define MAX_OFFSET 39

#define TRUE 1
#define FALSE 0

enum motorState_e {
  FORWARD,
  REVERSE,
  ROTATE,
  LEFT,
  RIGHT,
  STOP
};

struct motorSettings_t {
  motorState_e state;
  float targetAngle;
  long int targetTimeMs;
  float target;
};

struct encoder_t {
    long int tickRS;
    long int tickLS;
};

struct pwrCtrl_t {
    long int lastUpdateTime;
    int powerLS = MOTOR_PWR;
    int powerRS = MOTOR_PWR;
    int prevCntLS = 0;
    int prevCntRS = 0;
};

struct pos {
  float X;
  float Y;
  float Angle;
};

class MotorControl
{
  public:
    MotorControl();
    void setMotor(motorSettings_t newSettings);
    IMU imu;
    int yaw;
    int update();
    void init();
    bool ready;
  private:
    motorSettings_t settings;
    pwrCtrl_t pwrCtrl;
    void forward();
    void reverse();
    void left();
    void right();
    void stop();
    void initEncoderPins();
    float getDistance();
    void resetEncoders();
    void updatePwrCtrl();
};

#endif
