#include "Arduino.h"
#include "MotorControl.h"

MotorControl::MotorControl()
{
  pinMode(LEFT_FWD_PIN,  OUTPUT);
  pinMode(LEFT_BWD_PIN,  OUTPUT);
  pinMode(RIGHT_FWD_PIN, OUTPUT);
  pinMode(RIGHT_BWD_PIN, OUTPUT);
}

void MotorControl::setMotor(motorState_e newState, float percentOutput,
                                                   float turnSensitivity)
{
  settings.state = newState;
  if (0 <= percentOutput <= 1) {
    settings.outputPwr = percentOutput * 255;
  }
  if (0 <= turnSensitivity <= 1) {
    settings.secondaryPwr = 255 - (2 * 255) * turnSensitivity;
  }
  updateMotorCtrlPins();
}

void MotorControl::updateMotorCtrlPins()
{
  switch (settings.state) {
    case FORWARD:
      forward();
      break;
    case REVERSE:
      reverse();
      break;
    case STEER_LEFT:
      steerLeft();
      break;
    case STEER_RIGHT:
      steerRight();
      break;
    case STOP:
      stop();
      break;
  }
}

void MotorControl::forward()
{
  analogWrite(LEFT_FWD_PIN,  settings.outputPwr);
  analogWrite(LEFT_BWD_PIN,  LOW);
  analogWrite(RIGHT_FWD_PIN, settings.outputPwr);
  analogWrite(RIGHT_BWD_PIN, LOW);
}

void MotorControl::reverse()
{
  analogWrite(LEFT_FWD_PIN,  LOW);
  analogWrite(LEFT_BWD_PIN,  settings.outputPwr);
  analogWrite(RIGHT_FWD_PIN, LOW);
  analogWrite(RIGHT_BWD_PIN, settings.outputPwr);
}

void MotorControl::steerLeft()
{
  if (settings.secondaryPwr > 0) {
    analogWrite(LEFT_FWD_PIN,  settings.secondaryPwr);
    analogWrite(LEFT_BWD_PIN,  LOW);
  }
  else {
    analogWrite(LEFT_FWD_PIN,  LOW);
    analogWrite(LEFT_BWD_PIN,  settings.secondaryPwr);
  }
  analogWrite(RIGHT_FWD_PIN, settings.outputPwr);
  analogWrite(RIGHT_BWD_PIN, LOW);
}

void MotorControl::steerRight()
{
  if (settings.secondaryPwr > 0) {
    analogWrite(RIGHT_FWD_PIN, settings.secondaryPwr);
    analogWrite(RIGHT_BWD_PIN, LOW);
  }
  else {
    analogWrite(RIGHT_FWD_PIN, LOW);
    analogWrite(RIGHT_BWD_PIN, settings.secondaryPwr);
  }
  analogWrite(LEFT_FWD_PIN, settings.outputPwr);
  analogWrite(LEFT_BWD_PIN, LOW);
}

void MotorControl::stop()
{
  analogWrite(LEFT_FWD_PIN,  LOW);
  analogWrite(LEFT_BWD_PIN,  LOW);
  analogWrite(RIGHT_FWD_PIN, LOW);
  analogWrite(RIGHT_BWD_PIN, LOW);
}
