
#ifndef H_TELE
#define H_TELE

#include "esp8266-motor.h"
#include "esp8266-dht.h"

#include <ArduinoJson.h>
#include <BearSSLHelpers.h>
#include <ESP8266mDNS.h>
#include <ESP8266WiFi.h>
#include <UniversalTelegramBot.h>
#include <vector>
#include <WiFiClientSecure.h>

namespace ESP8266Hanger {
class TelegramControl {
private:
  String ssid;
  String password;
  String hostname;
  String botToken;
  std::vector<String> acceptedUserIds;

  WiFiClientSecure wifiClient;
  X509List *cert;

  MotorControl *motorControl;
  DHTControl *dhtControl;

  UniversalTelegramBot *bot;
  unsigned long lastBotHandled;
  int botRequestDelay;

  void handleNewMessage(int);

public:
  TelegramControl();

  void setMotorControl(MotorControl *);

  void setDHTControl(DHTControl *);

  void setupNetworking();

  void handleNewMessages();
};

template<typename T>
bool vectorContains(std::vector<T> vec, T element) {
  return (std::find(vec.begin(), vec.end(), element) != vec.end());
}

}

#endif