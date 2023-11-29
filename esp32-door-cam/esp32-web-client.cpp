#include "WiFiClientSecure.h"
#include "esp_camera.h"
#include "esp32-web-client.h"

WiFiClientSecure client;
WiFiClientSecure clientTele;

std::vector<String> acceptedIds = SECRET_TELEGRAM_IDS;

enum Code {
  TELE_CONN_FAIL = 1,
  CAMERA_READ_ERR = 2
};

WebClient::WebClient() {
  this->lastRefresh = 0;

  this->clientTeleListener.setInsecure();
  this->bot = new UniversalTelegramBot(SECRET_TELEGRAM_BOT_TOKEN, this->clientTeleListener);
  this->lastBotHandled = 0;
  this->botRequestDelay = 1000;
  this->acceptedIds = SECRET_TELEGRAM_IDS;

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

    for (String chatId : acceptedIds) {
      this->broadcastMessage("up and running");
    }
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

  DynamicJsonDocument jsonDoc(1024);
  DeserializationError err = deserializeJson(jsonDoc, payload);
  if (err) return;

  for (String chatId : acceptedIds) {
    sendPhotoTelegram(chatId, jsonDoc["message"].as<String>());
  }
}

int sendPhotoTelegram(String chatId, String msg) {
  const String myDomain = "api.telegram.org";
  String getAll = "";
  String getBody = "";

  camera_fb_t* fb = NULL;
  fb = esp_camera_fb_get();
  if (!fb) {
    Serial.println("Camera capture failed");
    delay(1000);
    ESP.restart();
    return Code::CAMERA_READ_ERR;
  }
  Serial.println("Connecting to " + myDomain);
  if (clientTele.connect(myDomain.c_str(), 443)) {
    Serial.println("Connection successful");

    String head = "--cam\r\nContent-Disposition: form-data; name=\"chat_id\"; \r\n\r\n" + chatId + "\r\n";
    if (msg.length() > 0) head += "--cam\r\nContent-Disposition: form-data; name=\"caption\"\r\n\r\n[Smart Lock System] " + msg + "\r\n";
    head += "--cam\r\nContent-Disposition: form-data; name=\"photo\"; filename=\"esp32-cam.jpg\"\r\nContent-Type: image/jpeg\r\n\r\n";
    String tail = "\r\n--cam--\r\n";

    uint16_t imageLen = fb->len;
    uint16_t extraLen = head.length() + tail.length();
    uint16_t totalLen = imageLen + extraLen;

    clientTele.println("POST /bot" + String(SECRET_TELEGRAM_BOT_TOKEN) + "/sendPhoto HTTP/1.1");
    clientTele.println("Host: " + myDomain);
    clientTele.println("Content-Length: " + String(totalLen));
    clientTele.println("Content-Type: multipart/form-data; boundary=cam");
    clientTele.println();
    clientTele.print(head);

    uint8_t* fbBuf = fb->buf;
    size_t fbLen = fb->len;
    for (size_t n = 0; n < fbLen; n = n + 1024) {
      if ((n + 1024) < fbLen) {
        clientTele.write(fbBuf, 1024);
        fbBuf += 1024;
      } else if (fbLen % 1024 > 0) {
        size_t remainder = fbLen % 1024;
        clientTele.write(fbBuf, remainder);
      }
    }

    clientTele.print(tail);

    esp_camera_fb_return(fb);

    int waitTime = 100000;  // timeout 100 seconds
    long startTimer = millis();
    boolean state = false;

    while ((startTimer + waitTime) > millis()) {
      Serial.print(".");
      delay(1000);
      while (clientTele.available()) {
        char c = clientTele.read();
        if (state == true) getBody += String(c);
        if (c == '\n') {
          if (getAll.length() == 0) state = true;
          getAll = "";
        } else if (c != '\r')
          getAll += String(c);
        startTimer = millis();
      }
      if (getBody.length() > 0) break;
    }
    clientTele.stop();
    Serial.println(getBody);
  } else {
    getBody = "Connected to api.telegram.org failed.";
    Serial.println("Connected to api.telegram.org failed.");
    return Code::TELE_CONN_FAIL;
  }
  return 0;
}

void WebClient::handleNewMessages() {
  if (millis() > (this->lastBotHandled + this->botRequestDelay)) {
    int numNewMsgs = this->bot->getUpdates(this->bot->last_message_received + 1);

    while (numNewMsgs) {
      Serial.println("Got response");
      this->handleNewMessage(numNewMsgs);
      numNewMsgs = this->bot->getUpdates(this->bot->last_message_received + 1);
    }

    this->lastBotHandled = millis();
  }
}

void WebClient::handleNewMessage(int num) {
  Serial.println("Handling new message");
  Serial.println(String(num));

  for (int i = 0; i < num; ++i) {
    telegramMessage msg = this->bot->messages[i];
    String chatId = String(msg.chat_id);
    String name = msg.from_name;
    String text = msg.text;

    if (!vectorContains(this->acceptedIds, chatId)) {
      this->bot->sendMessage(chatId, "You are an unauthorised user!");
      continue;
    }
  }
}

void WebClient::broadcastMessage(String msg) {
  String prepend = "[Smart Lock Camera]: ";
  for (String chatId : this->acceptedIds) {
    this->bot->sendMessage(chatId, prepend + msg);
  }
}
