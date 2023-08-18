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
void sendPhotoTelegram(String);

class WebClient {
private:
  MQTTClient *mqttClient;
  unsigned long lastRefresh;

public:
  WebClient();

  /** Creates a connection to AWS IoT Core */
  void connectAWS();

  /** Keeps the MQTT connection alive */
  void refreshConnection();
};

#endif