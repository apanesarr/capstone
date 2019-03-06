#include "CarMotorCtrl.h"
#include "MotorPatterns.h"
#include "RadioComms.h"



#define ARRAY_SIZE(a) (sizeof(a) / sizeof(a[0])) 
MotorControl motor;
int i = 0;
motorSettings_t getNewMotorSettings();
Antenna antenna;

motorSettings_t settings[LEN_PATTERN_2] = {TEST_PATTERN_2};

void setup() {
  //motor.init();
  antenna.init();
}

void loop() {
  antenna.update();
  //motor.update();
//  if (motor.ready) {
//    motor.setMotor(getNewMotorSettings());
//  }
//  delay(10);
}

motorSettings_t getNewMotorSettings()
{
  motorSettings_t newSettings = settings[i];
  i = (i + 1) % ARRAY_SIZE(settings);
  return newSettings;
}
