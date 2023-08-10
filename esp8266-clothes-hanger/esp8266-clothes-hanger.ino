#include "esp8266-motor.h"
#include "esp8266-dht.h"
#include "esp8266-telegram.h"

#define RAIN_HUMIDITY_LOWER_BOUND 90
#define SUN_HUMIDITY_UPPER_BOUND 80

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

  if (hum >= RAIN_HUMIDITY_LOWER_BOUND) {
    bool changed = motorControl.retractMotor();
    if (changed)
      telegramControl.broadcastMessage("Rain detected and clothes hanger has been retracted!");
  } else if (hum <= SUN_HUMIDITY_UPPER_BOUND) {
    bool changed = motorControl.extendMotor();
    if (changed)
      telegramControl.broadcastMessage("Rain has subsided and clothes hanger has been extended!");
  }

  telegramControl.handleNewMessages();
  delay(500);
}