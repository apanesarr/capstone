#include "Arduino.h"
#include "CarMotorCtrl.h"

static encoder_t encoder;

void updateEncLS()
{
    encoder.tickLS++;
}

void updateEncRS()
{
    encoder.tickRS++;
}

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
    Serial.begin(115200);
    initEncoderPins();
    ready = TRUE;
}

void MotorControl::initEncoderPins()
{
    /* Right side encoder */
    pinMode(RS_ENC_A, INPUT);
    digitalWrite(RS_ENC_A, HIGH);
    attachInterrupt(digitalPinToInterrupt(RS_ENC_A), updateEncRS, CHANGE);
    DEBUG_PRINT("ISR num:", digitalPinToInterrupt(RS_ENC_A));

    /* Left side encoder */
    pinMode(LS_ENC_A, INPUT);
    digitalWrite(LS_ENC_A, HIGH);
    attachInterrupt(digitalPinToInterrupt(LS_ENC_A), updateEncLS, CHANGE);
}

void MotorControl::setMotor(motorSettings_t newSettings)
{
  settings.state = newSettings.state;
  ready = FALSE;

  switch (settings.state) {
    case FORWARD:
      settings.target = newSettings.target;
      resetEncoders();
      forward();
      break;
    case REVERSE:
      settings.target = newSettings.target;
      resetEncoders();
      reverse();
      break;
    case LEFT:
      settings.target = imu.getYaw() - newSettings.target;
      left();
      break;
    case RIGHT:
      settings.target = imu.getYaw() + newSettings.target;
      right();
      break;
    case STOP:
      stop();
      break;
  }
}

int MotorControl::update()
{

  imu.update();

  switch (settings.state) {
    case FORWARD:
    case REVERSE:
        if (getDistance() >= settings.target) {
            stop();
            ready = TRUE;
        }
        else if ((millis() - pwrCtrl.lastUpdateTime) > UPDATE_RATE) {
            updatePwrCtrl();
        }
        break;

    case STOP:
      ready = TRUE;
      break;

    case LEFT:
      if (imu.getYaw() <= settings.target) {
        stop();
        ready = TRUE;
      }
      break;

    case RIGHT:
      if (imu.getYaw() >= settings.target) {
        stop();
        ready = TRUE;
      }
      break;
  }
  return 1;
}

void MotorControl::updatePwrCtrl()
{
    int diffLS, diffRS;

    diffRS = encoder.tickRS - pwrCtrl.prevCntRS;
    diffLS = encoder.tickLS - pwrCtrl.prevCntLS;
    pwrCtrl.prevCntRS = encoder.tickRS;
    pwrCtrl.prevCntLS = encoder.tickLS;

    if (diffLS > diffRS) {
        pwrCtrl.powerLS -= MOTOR_OFFSET;
        pwrCtrl.powerRS += MOTOR_OFFSET;
    }
    else if (diffLS < diffRS) {
        pwrCtrl.powerLS += MOTOR_OFFSET;
        pwrCtrl.powerRS -= MOTOR_OFFSET;
    }
    pwrCtrl.powerLS = constrain(pwrCtrl.powerLS, 0, 255);
    pwrCtrl.powerRS = constrain(pwrCtrl.powerRS, 0, 255);

    Serial.print("LS:\t");
    Serial.print(encoder.tickLS);
    Serial.print("\t");
    Serial.println(pwrCtrl.powerLS);

    Serial.print("RS:\t");
    Serial.print(encoder.tickRS);
    Serial.print("\t");
    Serial.println(pwrCtrl.powerRS);

    switch (settings.state) {
    case FORWARD:
        forward();
        break;
    case REVERSE:
        reverse();
        break;
    }
    pwrCtrl.lastUpdateTime = millis();
}

float MotorControl::getDistance()
{
    float avgTicks;
    float distance;

    avgTicks = (encoder.tickLS + encoder.tickRS) / 2;
    distance = 2 * M_PI * (avgTicks / TICKS_PER_REV) * WHEEL_RADIUS;
    return distance;
}

void MotorControl::resetEncoders()
{
    encoder.tickRS = 0;
    encoder.tickLS = 0;
    pwrCtrl.prevCntRS = 0;
    pwrCtrl.prevCntLS = 0;
}

void MotorControl::forward()
{
  analogWrite(LEFT_FWD_PIN,  pwrCtrl.powerLS);
  analogWrite(LEFT_BWD_PIN,  LOW);
  analogWrite(RIGHT_FWD_PIN, pwrCtrl.powerRS);
  analogWrite(RIGHT_BWD_PIN, LOW);
}

void MotorControl::reverse()
{
  analogWrite(LEFT_FWD_PIN,  LOW);
  analogWrite(LEFT_BWD_PIN,  pwrCtrl.powerLS);
  analogWrite(RIGHT_FWD_PIN, LOW);
  analogWrite(RIGHT_BWD_PIN, pwrCtrl.powerRS);
}

void MotorControl::left()
{
  analogWrite(RIGHT_FWD_PIN, pwrCtrl.powerRS);
  analogWrite(RIGHT_BWD_PIN, LOW);
  analogWrite(LEFT_FWD_PIN,  LOW);
  analogWrite(LEFT_BWD_PIN,  pwrCtrl.powerLS);
}

void MotorControl::right()
{
  analogWrite(LEFT_FWD_PIN, pwrCtrl.powerLS);
  analogWrite(LEFT_BWD_PIN, LOW);
  analogWrite(RIGHT_BWD_PIN, pwrCtrl.powerRS);
  analogWrite(RIGHT_FWD_PIN, LOW);
}

void MotorControl::stop()
{
  analogWrite(LEFT_FWD_PIN,  LOW);
  analogWrite(LEFT_BWD_PIN,  LOW);
  analogWrite(RIGHT_FWD_PIN, LOW);
  analogWrite(RIGHT_BWD_PIN, LOW);
}
