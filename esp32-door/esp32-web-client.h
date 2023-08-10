#ifndef H_WEB_CLIENT
#define H_WEB_CLIENT

#include <ArduinoJson.h>
#include <MQTTClient.h>
#include <WiFi.h>
#include <WiFiClientSecure.h>

#include "secrets.h"

#define AWS_IOT_PUBLISH_TOPIC "esp32/door_invalid_entry"

namespace ESP32Door {

class WebClient {
private:
  WiFiClientSecure client;
  MQTTClient *mqttClient;
  unsigned long lastRefresh;

public:
  WebClient();

  /** Creates a connection to AWS IoT Core */
  void connectAWS();

  /** Keeps the MQTT connection alive */
  void refreshConnection();

  /** Publishes a message when invalid PIN/NFC device was provided */
  void publishInvalidEntry();
};

}

#endif