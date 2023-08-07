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

  float readTemperature();

  float readHumidity();
};
}

#endif