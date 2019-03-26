#include "CarMotorCtrl.h"
#include "MotorPatterns.h"
#include "RadioComms.h"
#include "dht.h"

MotorControl motor;

RadioComms comms;

void setup() {
  // motor.init();
  comms.init(1);
}

void loop() {
  comms.update();
  // motor.update();
  // if (motor.ready) {
  //   motor.setMotor(comms.newSettings);
  // }
  delay(10);
}
