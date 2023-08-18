#include "esp8266-motor.h"
#include "esp8266-weather.h"
#include "esp8266-telegram.h"

#define RAIN_LEVEL_UPPER_BOUND 512
#define RAIN_LEVEL_LOWER_BOUND 768

ESP8266Hanger::MotorControl motorControl;
ESP8266Hanger::WeatherControl weatherControl;
ESP8266Hanger::TelegramControl telegramControl;

void setup(void) {
  Serial.begin(115200);
  telegramControl.setupNetworking();
  telegramControl.setMotorControl(&motorControl);
  telegramControl.setWeatherControl(&weatherControl);

  // Inform users on system initialisation
  telegramControl.broadcastMessage("Clothes hanger system up and running");
}

void loop() {
  int rainLevel =  weatherControl.readRainLevel();

  if (rainLevel <= RAIN_LEVEL_UPPER_BOUND) {
    bool changed = motorControl.retractMotor();
    if (changed)
      telegramControl.broadcastMessage("Rain detected and clothes hanger has been retracted!");
  } else if (rainLevel >= RAIN_LEVEL_LOWER_BOUND) {
    bool changed = motorControl.extendMotor();
    if (changed)
      telegramControl.broadcastMessage("Rain has subsided and clothes hanger has been extended!");
  }

  telegramControl.handleNewMessages();
  delay(500);
}