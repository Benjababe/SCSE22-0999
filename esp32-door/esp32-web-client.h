#ifndef H_WEB_CLIENT
#define H_WEB_CLIENT

#include <ArduinoJson.h>
#include <MQTTClient.h>
#include <UniversalTelegramBot.h>
#include <WiFi.h>
#include <WiFiClientSecure.h>

#include "secrets.h"

#define AWS_IOT_PUBLISH_TOPIC "esp32/door_invalid_entry"

namespace ESP32Door {

class WebClient {
private:
  MQTTClient *mqttClient;
  unsigned long lastRefresh;

public:
  WiFiClientSecure client;
  WiFiClientSecure teleClient;

  WebClient();

  /** Creates a connection to AWS IoT Core */
  void connectAWS();

  /** Keeps the MQTT connection alive */
  void refreshConnection();

  /** Publishes a message when invalid PIN/NFC device was provided */
  void publishInvalidEntry();

  /** Publishes a message when /snapshot command is provided */
  void publishSnapshot(bool);
};

}

#endif