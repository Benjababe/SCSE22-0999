#include "esp8266-motor.h"
#include "esp8266-weather.h"
#include "esp8266-telegram.h"

ESP8266Hanger::MotorControl motorControl;
ESP8266Hanger::WeatherControl weatherControl;
ESP8266Hanger::TelegramControl telegramControl;

void setup(void) {
  Serial.begin(115200);
  telegramControl.setupNetworking();
  telegramControl.setMotorControl(&motorControl);
  telegramControl.setWeatherControl(&weatherControl);

  // Inform users on system initialisation
  telegramControl.broadcastMessage("Up and running");
}

void loop() {
  int rainLevel = weatherControl.readRainLevel();

  if (rainLevel <= RAIN_LEVEL_UPPER_BOUND) {
    bool changed = motorControl.retractMotor(false);
    if (changed)
      telegramControl.broadcastMessage("Rain detected and clothes hanger has been retracted!");
  } else if (rainLevel >= RAIN_LEVEL_LOWER_BOUND && !motorControl.getManual()) {
    bool changed = motorControl.extendMotor(false);
    if (changed)
      telegramControl.broadcastMessage("Rain has subsided and clothes hanger has been extended!");
  }

  telegramControl.handleNewMessages();
  delay(500);
}