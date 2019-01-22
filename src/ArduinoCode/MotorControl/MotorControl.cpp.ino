#include "Arduino.h"
#include "MotorControl.h"

MotorControl::MotorControl() {
  pinMode(LEFT_PWR_PIN , OUTPUT);
  pinMode(RIGHT_PWR_PIN , OUTPUT);
  pinMode(LEFT_FWD_PIN , OUTPUT);
  pinMode(LEFT_BWD_PIN , OUTPUT);
  pinMode(RIGHT_FWD_PIN , OUTPUT);
  pinMode(RIGHT_BWD_PIN , OUTPUT);

}

void MotorControl::setMotorState(motorState_t newState) {
  motorState = newState;
  updateMotorState();
}

void MotorControl::updateMotorState() {
  switch (motorState.state) {
    case FORWARD:
      forward();
      break;
    case REVERSE:
      reverse();
      break;
    case STEERLEFT:
      steerLeft()
      break;
    case STEERRIGHT:
      steerRight();
      break;
    case STOPPED:
      stopMotor();
      break;
  }
}

void MotorControl::forward() {
  analogWrite(LEFT_PWR_PIN, (int)motorState.ratio*255);
  analogWrite(RIGHT_PWR_PIN, (int)motorState.ratio*255);
  digitalWrite(LEFT_FWD_PIN,HIGH);
  digitalWrite(RIGHT_FWD_PIN, HIGH);
}

void MotorControl::reverse() {
}

void MotorControl::steerLeft() {}

void MotorControl::steerRight() {}

void MotorControl::stopped() {}

//    void loop() {
//
//      switch () {
//
//      }
//      // put your main code here, to run repeatedly:
//      digitalWrite(leftForward , HIGH);
//      digitalWrite(leftBackward , LOW);
//      digitalWrite(rightForward , HIGH);
//      digitalWrite(rightBackward , LOW);
//
//    }
