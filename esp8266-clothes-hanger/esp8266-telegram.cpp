#include "esp8266-telegram.h"

namespace ESP8266Hanger {

TelegramControl::TelegramControl() {
  // Replace values accordingly
  this->ssid = "BenjaF3";
  this->password = "benjamin";
  this->hostname = "esp8266-hanger";
  this->botToken = "6323726791:AAFnKyfokJ2Cs8FVFvSQGf5ng0dizhJfdw4";
  this->acceptedUserIds.push_back("665251898");

  this->lastBotHandled = 0;
  this->botRequestDelay = 1000;
  this->wifiClient.setInsecure();
  this->cert = new X509List(TELEGRAM_CERTIFICATE_ROOT);
  this->bot = new UniversalTelegramBot(botToken, this->wifiClient);
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

void TelegramControl::setDHTControl(DHTControl *dhtControl) {
  this->dhtControl = dhtControl;
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

    if (!vectorContains(this->acceptedUserIds, chatId)) {
      this->bot->sendMessage(chatId, "You are an unauthorised user!");
      continue;
    }

    if (text == "/start") {
      String msg = "Welcome, " + name + ".\n";
      msg += "Use the following command to get current state of the clothes hanger.\n\n";
      msg += "/state \n";
      this->bot->sendMessage(chatId, msg);
    }

    else if (text == "/state") {
      MotorState state = this->motorControl->getState();
      float humidity = this->dhtControl->readHumidity();
      float temperature = this->dhtControl->readTemperature();

      String msg = "Current state of clothes hanger: \n";
      msg += "Hanger: " + String((state == MotorState::extended) ? "Extended" : "Retracted") + "\n";
      msg += "Humidity: " + String(humidity) + "%\n";
      msg += "Temperature: " + String(temperature) + "ºC\n";
      this->bot->sendMessage(chatId, msg);
    }
  }
}

}