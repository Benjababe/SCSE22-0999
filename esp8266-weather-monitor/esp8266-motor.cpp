#include "esp8266-motor.h"

#define INA D0
#define INB D1

namespace ESP8266Hanger {

MotorControl::MotorControl() {
  pinMode(INA, OUTPUT);
  pinMode(INB, OUTPUT);

  this->extendMotor(false);
}

MotorState MotorControl::getState() {
  return this->state;
}

bool MotorControl::getManual() {
  return this->manual;
}

bool MotorControl::extendMotor(bool manual) {
  if (this->state == MotorState::extended)
    return false;

  this->manual = manual;

  digitalWrite(INA, HIGH);
  digitalWrite(INB, LOW);
  delay(1200);
  this->state = MotorState::extended;

  this->stop();
  Serial.println("Extended!");
  return true;
}

bool MotorControl::retractMotor(bool manual) {
  if (this->state == MotorState::retracted)
    return false;

  this->manual = manual;

  digitalWrite(INA, LOW);
  digitalWrite(INB, HIGH);
  delay(1200);
  this->state = MotorState::retracted;

  this->stop();
  Serial.println("Retracted!");
  return true;
}

void MotorControl::stop() {
  digitalWrite(INA, LOW);
  digitalWrite(INB, LOW);
}

}