#include "Arduino.h"
#include "CarMotorCtrl.h"

MotorControl::MotorControl()
{
  pinMode(LEFT_FWD_PIN,  OUTPUT);
  pinMode(LEFT_BWD_PIN,  OUTPUT);
  pinMode(RIGHT_FWD_PIN, OUTPUT);
  pinMode(RIGHT_BWD_PIN, OUTPUT);
}

void MotorControl::setMotor(motorState_e newState)
{
  state = newState;
  updateMotorCtrlPins();
}

void MotorControl::updateMotorCtrlPins()
{
  switch (state) {
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

void MotorControl::steerLeft()
{
  analogWrite(RIGHT_FWD_PIN, RIGHT_MOTOR_PWR);
  analogWrite(RIGHT_BWD_PIN, LOW);
  analogWrite(LEFT_FWD_PIN,  LOW);
  analogWrite(LEFT_BWD_PIN,  LEFT_MOTOR_PWR);
}

void MotorControl::steerRight()
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
