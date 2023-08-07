#ifndef H_DHT
#define H_DHT

#include <Arduino.h>
#include <DHT.h>

namespace ESP8266Hanger {
class DHTControl {
private:
  DHT *dht;

public:
  DHTControl();

  /** Returns current temperature reading from DHT11 in celsius */
  float readTemperature();

  /** Returns current humidity reading from DHT11 in percentage */
  float readHumidity();
};
}

#endif