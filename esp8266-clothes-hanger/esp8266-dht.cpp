#include "esp8266-dht.h"

namespace ESP8266Hanger {

DHTControl::DHTControl() {
  this->dht = new DHT(D2, DHT11);
  this->dht->begin();
}

float DHTControl::readTemperature() {
  return this->dht->readTemperature();
}

float DHTControl::readHumidity() {
  return this->dht->readHumidity();
}

}