#include "esp32-servo.h"

namespace ESP32Door {

ServoControl::ServoControl() {
  this->LOCK_ANGLE = 0;
  this->UNLOCK_ANGLE = 140;
  this->SERVO_DATA_PIN = 13;
  this->servo.attach(this->SERVO_DATA_PIN);
}

void ServoControl::lock() {
  this->servo.write(this->LOCK_ANGLE);
  delay(1000);
}

void ServoControl::unlock() {
  this->servo.write(this->UNLOCK_ANGLE);
  delay(1000);
}

void ServoControl::setAngle(int angle) {
  this->servo.write(angle);
  Serial.print("Moving to ");
  Serial.println(angle);
  delay(1000);
}

int ServoControl::getAngle() {
  return this->servo.read();
}

bool ServoControl::isLocked() {
  return (this->getAngle() == this->LOCK_ANGLE);
}

}