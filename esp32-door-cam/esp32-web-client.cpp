#include "WiFiClientSecure.h"
#include "esp_camera.h"
#include "esp32-web-client.h"

WiFiClientSecure client;
WiFiClientSecure clientTele;
UniversalTelegramBot bot(SECRET_TELEGRAM_BOT_TOKEN, clientTele);
ImgurUploader imgurUploader(SECRET_IMGUR_CLIENT_ID);
File img;

std::vector<String> acceptedIds = SECRET_TELEGRAM_IDS;

WebClient::WebClient() {
  this->lastRefresh = 0;

  clientTele.setCACert(TELEGRAM_CERTIFICATE_ROOT);
  client.setCACert(SECRET_AWS_CERT_CA);
  client.setCertificate(SECRET_AWS_CERT_CRT);
  client.setPrivateKey(SECRET_AWS_CERT_PRIVATE);

  this->mqttClient = new MQTTClient(256);
}

void WebClient::connectAWS() {
  // Wait for wifi to connect. This should only be run after web server setup
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.println("wtf");
  }

  this->mqttClient->begin(SECRET_AWS_IOT_ENDPOINT, 8883, client);

  Serial.print("Connecting to AWS IOT Core");
  while (!this->mqttClient->connect(SECRET_AWS_IOT_CLIENT_ID)) {
    Serial.print(".");
    delay(200);
  }

  if (!this->mqttClient->connected())
    Serial.println("\nAWS IOT Core timed out!");
  else {
    this->mqttClient->onMessage(onInvalidEntry);
    this->mqttClient->subscribe(AWS_IOT_SUBSCRIBE_TOPIC);
    Serial.println("\nAWS IOT Core connected!");
  }
}

void WebClient::refreshConnection() {
  // Sometimes image uploads take too long which causes the MQTT client to timeout
  if (!this->mqttClient->connected()) {
    this->connectAWS();
    this->lastRefresh = 0;
  } else {
    unsigned long timeSinceLast = millis() - this->lastRefresh;
    if (timeSinceLast >= 3000) {
      this->mqttClient->loop();
      this->lastRefresh = millis();
    }
  }
}

void onInvalidEntry(String& topic, String& payload) {
  Serial.println("Incoming: " + topic + " - " + payload);
  String imgPath = savePhoto();

  if (imgPath == "") {
    ESP.restart();
  }

  for (String chatId : acceptedIds) {
    sendPhotoTelegram(chatId, imgPath);
  }
}

String savePhoto() {
  // Take Picture with Camera
  camera_fb_t* fb = NULL;
  fb = esp_camera_fb_get();
  if (!fb) {
    Serial.println("Camera capture failed");
    return "";
  }

  EEPROM.begin(EEPROM_SIZE);
  int picNum = EEPROM.read(0) + 1;
  String path = "/pic_" + String(picNum) + ".jpg";
  fs::FS& fs = SD_MMC;
  Serial.printf("Picture file name: %s\n", path.c_str());

  // Write file to SD card
  File file = fs.open(path.c_str(), FILE_WRITE);
  if (!file) {
    Serial.println("Failed to open file in writing mode");
    return "";
  }

  file.write(fb->buf, fb->len);
  Serial.printf("Saved file to path: %s\n", path.c_str());
  EEPROM.write(0, picNum);
  EEPROM.commit();

  // Clean up
  file.close();
  esp_camera_fb_return(fb);

  return path;
}

bool isMoreDataAvailable() {
  return img.available();
}

byte getNextByte() {
  return img.read();
}

void sendPhotoTelegram(String chatId, String imgPath) {
  fs::FS& fs = SD_MMC;
  int ret = imgurUploader.uploadFile(fs, imgPath.c_str());

  if (ret > 0) {
    Serial.println("Imgur upload success");
    String url = String(imgurUploader.getURL());
    bot.sendMessage(chatId, "Invalid entry to lock, snapshot uploaded here: " + url);
  } else {
    Serial.println("Imgur upload failure");
    bot.sendMessage(chatId, "Invalid entry to lock, error with snapshot");
  }
}
