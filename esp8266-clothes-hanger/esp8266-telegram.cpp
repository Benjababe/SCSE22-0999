#include "esp8266-telegram.h"

namespace ESP8266Hanger {

TelegramControl::TelegramControl() {
  // Replace values accordingly
  this->ssid = SECRET_SSID;
  this->password = SECRET_PASSWORD;
  this->botToken = SECRET_TELEGRAM_BOT_TOKEN;
  this->hostname = "esp8266-hanger";
  this->acceptedIds.push_back("665251898");

  this->lastBotHandled = 0;
  this->botRequestDelay = 1000;
  this->wifiClient.setInsecure();
  this->cert = new X509List(TELEGRAM_CERTIFICATE_ROOT);
  this->bot = new UniversalTelegramBot(botToken, this->wifiClient);

  MDNS.addService("http", "tcp", 80);
}

void TelegramControl::setupNetworking() {
  WiFi.mode(WIFI_STA);
  WiFi.begin(this->ssid, this->password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println(WiFi.localIP());

  // Broadcast mDNS
  if (!MDNS.begin(hostname)) {
    Serial.println("Error starting mDNS");
  }

  // Sync NTP
  configTime(0, 0, "pool.ntp.org");

  // Add root certificate for api.telegram.org
  this->wifiClient.setTrustAnchors(cert);
}

void TelegramControl::setMotorControl(MotorControl *motorControl) {
  this->motorControl = motorControl;
}

void TelegramControl::setWeatherControl(WeatherControl *weatherControl) {
  this->weatherControl = weatherControl;
}

void TelegramControl::handleNewMessages() {
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

void TelegramControl::handleNewMessage(int num) {
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

    if (text == "/start") {
      String msg = "Welcome, " + name + ".\n";
      msg += "Use the following command to get current state of the clothes hanger.\n\n";
      msg += "/state \n";
      this->bot->sendMessage(chatId, msg);
    }

    else if (text == "/weather") {
      MotorState state = this->motorControl->getState();
      float humidity = this->weatherControl->readHumidity();
      float temperature = this->weatherControl->readTemperature();
      float rainLevelPctg = this->weatherControl->readRainLevelPctg();

      String msg = "Current state of clothes hanger: \n";
      msg += "Hanger: " + String((state == MotorState::extended) ? "Extended" : "Retracted") + "\n";
      msg += "Humidity: " + String(humidity) + "%\n";
      msg += "Temperature: " + String(temperature) + "ºC\n";
      msg += "Rain Level: " + String(rainLevelPctg) + "%\n";
      this->bot->sendMessage(chatId, msg);
    }
  }
}

void TelegramControl::broadcastMessage(String msg) {
  for (String chatId : this->acceptedIds) {
    this->bot->sendMessage(chatId, msg);
  }
}

}