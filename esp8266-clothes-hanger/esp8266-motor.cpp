#include "esp8266-motor.h"

namespace ESP8266Hanger {

MotorControl::MotorControl() {
  this->INA = D0;
  this->INB = D1;

  pinMode(this->INA, OUTPUT);
  pinMode(this->INB, OUTPUT);

  this->extendMotor();
}

MotorState MotorControl::getState() {
  return this->state;
}

void MotorControl::extendMotor() {
  if (this->state == MotorState::extended)
    return;

  digitalWrite(this->INA, HIGH);
  digitalWrite(this->INB, LOW);
  delay(1200);
  this->state = MotorState::extended;

  this->stop();
  Serial.println("Extended!");
}

void MotorControl::retractMotor() {
  if (this->state == MotorState::retracted)
    return;

  digitalWrite(this->INA, LOW);
  digitalWrite(this->INB, HIGH);
  delay(1200);
  this->state = MotorState::retracted;

  this->stop();
  Serial.println("Retracted!");
}

void MotorControl::stop() {
  digitalWrite(this->INA, LOW);
  digitalWrite(this->INB, LOW);
}

}