
#include "CarMotorCtrl.h"

MotorControl motor;
motorSettings_t settings;

void setup() {
  motor.init();
  settings.state = FORWARD;
  settings.target = 65000;
  motor.setMotor(settings);
}

void loop() {
  motor.update();
}
