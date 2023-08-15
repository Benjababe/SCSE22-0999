#ifndef H_WEB_CLIENT
#define H_WEB_CLIENT

#include <ArduinoJson.h>
#include <EEPROM.h>
#include <esp_camera.h>
#include "FS.h"
#include <ImgurUploader.h>
#include <MQTTClient.h>
#include "SD_MMC.h"
#include "soc/soc.h"
#include "soc/rtc_cntl_reg.h"
#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <UniversalTelegramBot.h>
#include <vector>

#include "secrets.h"

#define AWS_IOT_SUBSCRIBE_TOPIC "esp32/door_invalid_entry"
#define EEPROM_SIZE 1

bool isMoreDataAvailable();
byte getNextByte();
void onInvalidEntry(String &, String &);
String savePhoto();
void sendPhotoTelegram(String, String);


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