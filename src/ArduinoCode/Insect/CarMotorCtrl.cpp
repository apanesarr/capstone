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
    ready = TRUE;
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
