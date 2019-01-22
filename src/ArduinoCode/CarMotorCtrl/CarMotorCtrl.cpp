#include "Arduino.h"
#include "CarMotorCtrl.h"

MotorControl::MotorControl()
{
  pinMode(FWD_PIN,  OUTPUT);
  pinMode(BWD_PIN,  OUTPUT);
  pinMode(RIGHT_PIN, OUTPUT);
  pinMode(LEFT_PIN, OUTPUT);
}

void MotorControl::setMotor(motorState_e newState, float percentOutput)
{
  settings.state = newState;
  settings.outputPwr = percentOutput * 255;
  updateMotorCtrlPins();
}

void MotorControl::updateMotorCtrlPins()
{
  switch (settings.state) {
    case FORWARD:
      resetSteering();
      forward();
      break;
    case REVERSE:
      resetSteering();
      reverse();
      break;
    case FWD_LEFT:
      forward();
      left();
      break;
    case FWD_RIGHT:
      forward();
      right();
      break;
    case BACK_RIGHT:
      reverse();
      left();
    case BACK_LEFT:
      reverse();
      right();
    case STOP:
      stop();
      break;
    default:
      break;
  }
}

void MotorControl::forward()
{
  analogWrite(FWD_PIN,  settings.outputPwr);
  analogWrite(BWD_PIN,  LOW);
}

void MotorControl::reverse()
{
  analogWrite(FWD_PIN,  LOW);
  analogWrite(BWD_PIN,  settings.outputPwr);
}

void MotorControl::left()
{
  analogWrite(LEFT_PIN, 255);
  analogWrite(RIGHT_PIN, LOW);
}

void MotorControl::right()
{
  analogWrite(RIGHT_PIN, 255);
  analogWrite(LEFT_PIN, LOW);
}

void MotorControl::stop()
{
  analogWrite(FWD_PIN,  LOW);
  analogWrite(BWD_PIN,  LOW);
  analogWrite(RIGHT_PIN, LOW);
  analogWrite(LEFT_PIN, LOW);
}

void MotorControl::resetSteering()
{
  analogWrite(RIGHT_PIN, LOW);
  analogWrite(LEFT_PIN, LOW);
}
