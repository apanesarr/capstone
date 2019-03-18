
#include "CarMotorCtrl.h"

MotorControl motor;
motorSettings_t settings;

void setup() {
  motor.init();
  settings.state = LEFT;
  settings.target = 90;
  motor.setMotor(settings);
}

void loop() {
  motor.update();
}
