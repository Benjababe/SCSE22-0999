#ifndef H_WEB_CLIENT
#define H_WEB_CLIENT

#include <ArduinoJson.h>
#include <esp_camera.h>
#include <MQTTClient.h>
#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <UniversalTelegramBot.h>
#include <vector>

#include "secrets.h"

#define AWS_IOT_SUBSCRIBE_TOPIC "esp32/door_invalid_entry"

void onInvalidEntry(String &, String &);
int sendPhotoTelegram(String, String);

class WebClient {
private:
  MQTTClient *mqttClient;
  unsigned long lastRefresh;

  WiFiClientSecure clientTeleListener;
  UniversalTelegramBot *bot;
  unsigned long lastBotHandled;
  int botRequestDelay;
  std::vector<String> acceptedIds;

  /** Handles individual new message from user */
  void handleNewMessage(int);

  /** @brief Broadcast a given message to all accepted Ids */
  void broadcastMessage(String);

public:
  WebClient();

  /** Creates a connection to AWS IoT Core */
  void connectAWS();

  /** Keeps the MQTT connection alive */
  void refreshConnection();

  /** @brief Listens and handles any new telegram messages */
  void handleNewMessages();
};

/** Helper function to check if element exists in vector `vec` */
template<typename T>
bool vectorContains(std::vector<T> vec, T element) {
  return (std::find(vec.begin(), vec.end(), element) != vec.end());
}

#endif