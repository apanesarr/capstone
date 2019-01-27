#include <CarMotorCtrl.h>

MotorControl motor;

void setup() {}

void loop() {
  motor.setMotor(FWD_LEFT, 1);
  delay(1000);
  motor.setMotor(FWD_RIGHT, 1);
  delay(1000);
  motor.setMotor(FORWARD, 0.8);
  delay(1000);
  motor.setMotor(BACK_LEFT, 1);
  delay(1000);
  motor.setMotor(BACK_RIGHT, 1);
  delay(1000);
}
