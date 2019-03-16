#include "Arduino.h"
#include "CarMotorCtrl.h"

MotorControl::MotorControl()
{
  pinMode(LEFT_FWD_PIN,  OUTPUT);
  pinMode(LEFT_BWD_PIN,  OUTPUT);
  pinMode(RIGHT_FWD_PIN, OUTPUT);
  pinMode(RIGHT_BWD_PIN, OUTPUT);
}

void MotorControl::init()
{
    imu.init();
    initEncoderPins();
    ready = TRUE;
}

void MotorControl::initEncoderPins()
{
    /* Right side encoder */
    pinMode(RS_ENC_A, INPUT);
    digitalWrite(RS_ENC_A, HIGH);
    pinMode(RS_ENC_B, INPUT);
    digitalWrite(RS_ENC_B, HIGH);
    attachInterrupt(digitalPinToInterrupt(RS_ENC_A), updateEncRS, CHANGE);

    /* Left side encoder */
    pinMode(LS_ENC_A, INPUT);
    digitalWrite(LS_ENC_A, HIGH);
    pinMode(LS_ENC_B, INPUT);
    digitalWrite(LS_ENC_B, HIGH);
    attachInterrupt(digitalPinToInterrupt(LS_ENC_A), updateEncLS, CHANGE);
}

void MotorControl::setMotor(motorSettings_t newSettings)
{
  settings.state = newSettings.state;
  ready = FALSE;

  switch (settings.state) {
    case FORWARD:
      settings.targetTimeMs = millis() + newSettings.targetTimeMs;
      forward();
      break;
    case REVERSE:
      settings.targetTimeMs = millis() + newSettings.targetTimeMs;
      reverse();
      break;
    case LEFT:
      settings.targetAngle = imu.getYaw() - newSettings.targetAngle;
      left();
      break;
    case RIGHT:
      settings.targetAngle = imu.getYaw() + newSettings.targetAngle;
      right();
      break;
    case STOP:
      settings.targetTimeMs = millis() + newSettings.targetTimeMs;
      stop();
      break;
  }
}

int MotorControl::update()
{
  if (imu.update() < 0) {
      return -1;
  }
  switch (settings.state) {
    case FORWARD:
    case REVERSE:
    case STOP:
      if (millis() >= settings.targetTimeMs) {
          stop();
          ready = TRUE;
      }
      break;

    case LEFT:
      if (imu.getYaw() <= settings.targetAngle) {
        stop();
        ready = TRUE;
      }
      break;

    case RIGHT:
      if (imu.getYaw() >= settings.targetAngle) {
        stop();
        ready = TRUE;
      }
      break;
  }
  return 1;
}

void MotorControl::updateEncLS()
{
    if (digitalRead(LS_ENC_A) == digitalRead(LS_ENC_B)) {
        encoder.tickLS++;
    }
    else {
        encoder.tickLS--;
    }
}

void MotorControl::updateEncRS()
{
    if (digitalRead(RS_ENC_A) == digitalRead(RS_ENC_B)) {
        encoder.tickRS++;
    }
    else {
        encoder.tickRS--;
    }
}

void MotorControl::forward()
{
  analogWrite(LEFT_FWD_PIN,  LEFT_MOTOR_PWR);
  analogWrite(LEFT_BWD_PIN,  LOW);
  analogWrite(RIGHT_FWD_PIN, RIGHT_MOTOR_PWR);
  analogWrite(RIGHT_BWD_PIN, LOW);
}

void MotorControl::reverse()
{
  analogWrite(LEFT_FWD_PIN,  LOW);
  analogWrite(LEFT_BWD_PIN,  LEFT_MOTOR_PWR);
  analogWrite(RIGHT_FWD_PIN, LOW);
  analogWrite(RIGHT_BWD_PIN, RIGHT_MOTOR_PWR);
}

void MotorControl::left()
{
  analogWrite(RIGHT_FWD_PIN, RIGHT_MOTOR_PWR);
  analogWrite(RIGHT_BWD_PIN, LOW);
  analogWrite(LEFT_FWD_PIN,  LOW);
  analogWrite(LEFT_BWD_PIN,  LEFT_MOTOR_PWR);
}

void MotorControl::right()
{
  analogWrite(LEFT_FWD_PIN, LEFT_MOTOR_PWR);
  analogWrite(LEFT_BWD_PIN, LOW);
  analogWrite(RIGHT_BWD_PIN, RIGHT_MOTOR_PWR);
  analogWrite(RIGHT_FWD_PIN, LOW);
}

void MotorControl::stop()
{
  analogWrite(LEFT_FWD_PIN,  LOW);
  analogWrite(LEFT_BWD_PIN,  LOW);
  analogWrite(RIGHT_FWD_PIN, LOW);
  analogWrite(RIGHT_BWD_PIN, LOW);
}
