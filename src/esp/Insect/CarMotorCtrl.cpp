#include <ESP8266WiFi.h>
#include "CarMotorCtrl.h"

static encoder_t encoder;

void updateEncLS()
{
    encoder.tickLS++;
}

void updateEncRS()
{
    encoder.tickRS++;
}

MotorControl::MotorControl()
{
  pinMode(LEFT_FWD_PIN,  OUTPUT);
  pinMode(LEFT_BWD_PIN,  OUTPUT);
  pinMode(RIGHT_FWD_PIN, OUTPUT);
  pinMode(RIGHT_BWD_PIN, OUTPUT);
}

void MotorControl::init()
{
    analogWrite(LEFT_FWD_PIN, 0);
    analogWrite(LEFT_BWD_PIN, 0);
    analogWrite(RIGHT_FWD_PIN, 0);
    analogWrite(RIGHT_BWD_PIN, 0);
    imu.init();
    // Serial.begin(115200);
    initEncoderPins();
    ready = TRUE;
}

void MotorControl::initEncoderPins()
{
    /* Right side encoder */
    pinMode(RS_ENC_A, INPUT);
    digitalWrite(RS_ENC_A, HIGH);
    attachInterrupt(digitalPinToInterrupt(RS_ENC_A), updateEncRS, CHANGE);
    // DEBUG_PRINT("ISR num:", digitalPinToInterrupt(RS_ENC_A));

    /* Left side encoder */
    pinMode(LS_ENC_A, INPUT);
    digitalWrite(LS_ENC_A, HIGH);
    attachInterrupt(digitalPinToInterrupt(LS_ENC_A), updateEncLS, CHANGE);
}

void MotorControl::setMotor(motorSettings_t newSettings)
{
  settings.state = newSettings.state;
  ready = FALSE;

  switch (settings.state) {
    case FORWARD:
      settings.target = newSettings.target;
      resetEncoders();
      forward();
      ready = TRUE;
      break;
    case REVERSE:
      settings.target = newSettings.target;
      resetEncoders();
      reverse();
      ready = TRUE;
      break;
    case LEFT:
      settings.target = imu.getYaw() - newSettings.target;
      left();
      break;
    case RIGHT:
      settings.target = imu.getYaw() + newSettings.target;
      right();
      break;
    case STOP:
      stop();
      break;
  }
}

int MotorControl::update()
{

  imu.update();

  switch (settings.state) {
    case FORWARD:
    case REVERSE:
        if (getDistance() >= settings.target) {
            stop();
            ready = TRUE;
        }
        else if ((millis() - pwrCtrl.lastUpdateTime) > UPDATE_RATE) {
            updatePwrCtrl();
        }
        break;

    case STOP:
      ready = TRUE;
      break;

    case LEFT:
      if (imu.getYaw() <= settings.target) {
        stop();
        ready = TRUE;
      }
      break;

    case RIGHT:
      if (imu.getYaw() >= settings.target) {
        stop();
        ready = TRUE;
      }
      break;
  }
  return 1;
}

void MotorControl::updatePwrCtrl()
{
    int tickLS = encoder.tickLS;
    int tickRS = encoder.tickRS;
    int rateLS = encoder.tickLS - pwrCtrl.prevCntLS;
    int rateRS = encoder.tickRS - pwrCtrl.prevCntRS;
    pwrCtrl.prevCntLS = tickLS;
    pwrCtrl.prevCntRS = tickRS;

    int right = constrain(
                    round(
                        (tickLS - tickRS) * P_GAIN
                      + (rateLS - rateRS) * D_GAIN
                    ),
                    -MAX_OFFSET, MAX_OFFSET
                );
    int left  = - right;

    pwrCtrl.powerRS = round(RIGHT_OFFSET * MOTOR_PWR) + right;
    pwrCtrl.powerLS = MOTOR_PWR + left;
    pwrCtrl.powerLS = constrain(pwrCtrl.powerLS, 0, 1023);
    pwrCtrl.powerRS = constrain(pwrCtrl.powerRS, 0, 1023);

    // GRAPH_PRINT(tickLS, tickRS, pwrCtrl.powerLS, pwrCtrl.powerRS);

    switch (settings.state) {
    case FORWARD:
        forward();
        break;
    case REVERSE:
        reverse();
        break;
    }
    pwrCtrl.lastUpdateTime = millis();
}

float MotorControl::getDistance()
{
    float avgTicks;
    float distance;

    avgTicks = (encoder.tickLS + encoder.tickRS) / 2;
    distance = 2 * M_PI * (avgTicks / TICKS_PER_REV) * WHEEL_RADIUS;
    return distance;
}

void MotorControl::resetEncoders()
{
    encoder.tickRS = 0;
    encoder.tickLS = 0;
    pwrCtrl.prevCntRS = 0;
    pwrCtrl.prevCntLS = 0;
}

void MotorControl::forward()
{
  analogWrite(LEFT_FWD_PIN,  pwrCtrl.powerLS);
  analogWrite(LEFT_BWD_PIN,  LOW);
  analogWrite(RIGHT_FWD_PIN, pwrCtrl.powerRS);
  analogWrite(RIGHT_BWD_PIN, LOW);
}

void MotorControl::reverse()
{
  analogWrite(LEFT_FWD_PIN,  LOW);
  analogWrite(LEFT_BWD_PIN,  pwrCtrl.powerLS);
  analogWrite(RIGHT_FWD_PIN, LOW);
  analogWrite(RIGHT_BWD_PIN, pwrCtrl.powerRS);
}

void MotorControl::left()
{
  analogWrite(RIGHT_FWD_PIN, pwrCtrl.powerRS);
  analogWrite(RIGHT_BWD_PIN, LOW);
  analogWrite(LEFT_FWD_PIN,  LOW);
  analogWrite(LEFT_BWD_PIN,  pwrCtrl.powerLS);
}

void MotorControl::right()
{
  analogWrite(LEFT_FWD_PIN, pwrCtrl.powerLS);
  analogWrite(LEFT_BWD_PIN, LOW);
  analogWrite(RIGHT_BWD_PIN, pwrCtrl.powerRS);
  analogWrite(RIGHT_FWD_PIN, LOW);
}

void MotorControl::stop()
{
  analogWrite(LEFT_FWD_PIN,  LOW);
  analogWrite(LEFT_BWD_PIN,  LOW);
  analogWrite(RIGHT_FWD_PIN, LOW);
  analogWrite(RIGHT_BWD_PIN, LOW);
}
