#include "esp8266-motor.h"

#define INA D0
#define INB D1

namespace ESP8266Hanger {

MotorControl::MotorControl() {
  pinMode(INA, OUTPUT);
  pinMode(INB, OUTPUT);

  this->extendMotor();
}

MotorState MotorControl::getState() {
  return this->state;
}

bool MotorControl::extendMotor() {
  if (this->state == MotorState::extended)
    return false;

  digitalWrite(INA, HIGH);
  digitalWrite(INB, LOW);
  delay(1200);
  this->state = MotorState::extended;

  this->stop();
  Serial.println("Extended!");
  return true;
}

bool MotorControl::retractMotor() {
  if (this->state == MotorState::retracted)
    return false;

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