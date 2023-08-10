#include "esp32-web-client.h"

namespace ESP32Door {

WebClient::WebClient() {
  this->lastRefresh = 0;

  this->client.setCACert(SECRET_AWS_CERT_CA);
  this->client.setCertificate(SECRET_AWS_CERT_CRT);
  this->client.setPrivateKey(SECRET_AWS_CERT_PRIVATE);

  this->mqttClient = new MQTTClient(256);
}

void WebClient::connectAWS() {
  // Wait for wifi to connect. This should only be run after web server setup
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.println("wtf");
  }

  this->mqttClient->begin(SECRET_AWS_IOT_ENDPOINT, 8883, this->client);

  Serial.print("Connecting to AWS IOT Core");
  while (!this->mqttClient->connect(SECRET_THING_NAME)) {
    Serial.print(".");
    delay(200);
  }

  if (!this->mqttClient->connected())
    Serial.println("\nAWS IOT Core timed out!");
  else
    Serial.println("\nAWS IOT Core connected!");
}

void WebClient::refreshConnection() {
  unsigned long timeSinceLast = millis() - this->lastRefresh;
  if (timeSinceLast >= 1000) {
    this->mqttClient->loop();
    this->lastRefresh = millis();
  }
}

void WebClient::publishInvalidEntry() {
  StaticJsonDocument<200> doc;
  doc["time"] = millis();
  char jsonBuffer[512];
  serializeJson(doc, jsonBuffer);
  this->mqttClient->publish(AWS_IOT_PUBLISH_TOPIC, jsonBuffer);
}

}