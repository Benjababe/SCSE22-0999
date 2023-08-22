#include "esp8266-weather.h"

namespace ESP8266Hanger {

const int rainAnalogMin = 0;
const int rainAnalogMax = 1024;

WeatherControl::WeatherControl() {
  this->dht = new DHT(DHT_DAT, DHT11);
  this->dht->begin();
  pinMode(RAIN_D0, INPUT);
}

float WeatherControl::readTemperature() {
  return this->dht->readTemperature();
}

float WeatherControl::readHumidity() {
  return this->dht->readHumidity();
}

int WeatherControl::readRainLevel() {
  int rainAnalogVal = analogRead(RAIN_A0);
  return rainAnalogVal;
}

float WeatherControl::readRainLevelPctg() {
  float rainAnalogVal = (float) analogRead(RAIN_A0);
  return 100.0 - ((rainAnalogVal / 1024) * 100);
}

}