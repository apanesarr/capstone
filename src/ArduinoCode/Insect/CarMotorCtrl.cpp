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
  if (imu.update() < 0) {
      return -1;
  }
  switch (settings.state) {
    case FORWARD:
    case REVERSE:
        if (getDistance() >= settings.target) {
            stop();
            ready = TRUE;
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

float MotorControl::getDistance()
{
    float avgTicks;
    float distance;

    avgTicks = (encoder.tickLS + encoder.tickRS) / 2;
    distance = 2 * M_PI * (encoder.tickLS / TICKS_PER_REV) * WHEEL_RADIUS;
    //Serial.println(encoder.tickLS);
    return distance;
}

void MotorControl::resetEncoders()
{
    encoder.tickRS = 0;
    encoder.tickLS = 0;
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

void MotorControl::left()
{
  analogWrite(RIGHT_FWD_PIN, RIGHT_MOTOR_PWR);
  analogWrite(RIGHT_BWD_PIN, LOW);
  analogWrite(LEFT_FWD_PIN,  LOW);
  analogWrite(LEFT_BWD_PIN,  LEFT_MOTOR_PWR);
}

void MotorControl::right()
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
