
#ifndef H_TELE
#define H_TELE

#include "secrets.h"
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
  std::vector<String> acceptedIds;

  WiFiClientSecure wifiClient;
  X509List *cert;

  MotorControl *motorControl;
  DHTControl *dhtControl;

  UniversalTelegramBot *bot;
  unsigned long lastBotHandled;
  int botRequestDelay;

  /** Handles individual new message from user */
  void handleNewMessage(int);

public:
  TelegramControl();

  void setMotorControl(MotorControl *);

  void setDHTControl(DHTControl *);

  /** Initialises Wifi connection and mDNS broadcast */
  void setupNetworking();

  /** Listens and handles any new telegram messages */
  void handleNewMessages();

  /** Broadcast a given message to all accepted Ids */
  void broadcastMessage(String);
};

/** Helper function to check if element exists in vector `vec` */
template<typename T>
bool vectorContains(std::vector<T> vec, T element) {
  return (std::find(vec.begin(), vec.end(), element) != vec.end());
}

}

#endif