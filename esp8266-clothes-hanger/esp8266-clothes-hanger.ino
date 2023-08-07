#include "esp8266-motor.h"
#include "esp8266-dht.h"
#include "esp8266-telegram.h"

ESP8266Hanger::MotorControl motorControl;
ESP8266Hanger::DHTControl dhtControl;
ESP8266Hanger::TelegramControl telegramControl;

void setup(void) {
  Serial.begin(115200);
  telegramControl.setupNetworking();
  telegramControl.setMotorControl(&motorControl);
  telegramControl.setDHTControl(&dhtControl);
}

void loop() {
  float hum = dhtControl.readHumidity();

  if (hum >= 80)
    motorControl.retractMotor();
  else
    motorControl.extendMotor();

  telegramControl.handleNewMessages();

  delay(500);
}