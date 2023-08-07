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

  // Inform users on system initialisation
  telegramControl.broadcastMessage("Clothes hanger system up and running");
}

void loop() {
  float hum = dhtControl.readHumidity();

  if (hum >= 80) {
    bool changed = motorControl.retractMotor();
    if (changed)
      telegramControl.broadcastMessage("Rain detected and clothes hanger has been retracted!");
  } else if (hum <= 70) {
    bool changed = motorControl.extendMotor();
    if (changed)
      telegramControl.broadcastMessage("Rain has subsided and clothes hanger has been extended!");
  }

  telegramControl.handleNewMessages();
  delay(500);
}