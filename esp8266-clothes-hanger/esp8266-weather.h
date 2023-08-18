#ifndef H_DHT
#define H_DHT

#include <Arduino.h>
#include <DHT.h>

#define DHT_DAT D2
#define RAIN_D0 D3
#define RAIN_A0 A0

namespace ESP8266Hanger {
class WeatherControl {
private:
  DHT *dht;

public:
  WeatherControl();

  /** Returns current temperature reading from DHT11 in celsius */
  float readTemperature();

  /** Returns current humidity reading from DHT11 in percentage */
  float readHumidity();

  /** Returns current rain level in range 0-1024 */
  int readRainLevel();

  /** Rain level in a percentage of 1024 */
  float readRainLevelPctg();
};
}

#endif