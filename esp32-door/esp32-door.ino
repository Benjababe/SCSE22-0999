#include "esp32-web-server.h"
#include "esp32-web-client.h"
#include "esp32-servo.h"
#include "esp32-lock-control.h"
#include "esp32-nfc.h"

#define SHORT_PRESS_TIME 500
#define LONG_PRESS_TIME 3000

typedef unsigned long ulong;

const int BUTTON_PIN = 34;
int prevBtnState = LOW;
int btnState;
ulong pressedTime = 0;
ulong releasedTime = 0;

ESP32Door::WebServer *webServer;
ESP32Door::WebClient webClient;
ESP32Door::ServoControl servoCtrl;
ESP32Door::LockControl lockCtrl;
ESP32Door::NFCReader nfcReader;

void setup(void) {
  Serial.begin(115200);
  pinMode(BUTTON_PIN, INPUT);

  lockCtrl.setupLCD();
  lockCtrl.printLCDCenter("Initialising...");

  webServer = new ESP32Door::WebServer();
  lockCtrl.setWebServer(webServer);
  lockCtrl.setWebClient(&webClient);
  lockCtrl.setServoControl(&servoCtrl);
  lockCtrl.setNFCReader(&nfcReader);
  lockCtrl.setupKeypad();

  servoCtrl.lock();
  lockCtrl.clearPinEntry();

  webClient.connectAWS();
}

void loop() {
  webServer->listenClients(lockCtrl);
  webClient.refreshConnection();

  lockCtrl.handleNewMessages();

  lockCtrl.readNFC();

  // only handle key if it's locked, waiting for PIN input
  char key = lockCtrl.readKeypad();
  if (key && lockCtrl.state == ESP32Door::State::locked) {
    lockCtrl.handlePinEntry(key);
  }

  btnState = digitalRead(BUTTON_PIN);

  // if button has been held for an extended period, show IP
  if (prevBtnState == HIGH && btnState == HIGH) {
    unsigned long pressedDuration = millis() - pressedTime;

    if (pressedDuration >= LONG_PRESS_TIME && lockCtrl.state != ESP32Door::State::showIP) {
      Serial.println("Long press");
      lockCtrl.printLCDCenter("Local IP:", webServer->getIPString().c_str());
      lockCtrl.state = ESP32Door::State::showIP;
    }
  }

  // button is pressed
  else if (prevBtnState == LOW && btnState == HIGH) {
    pressedTime = millis();
  }

  // button is released
  else if (prevBtnState == HIGH && btnState == LOW) {
    unsigned long pressedDuration = millis() - pressedTime;

    if (pressedDuration <= SHORT_PRESS_TIME) {
      Serial.println("Short press");

      // return to PIN display if currently showing IP
      if (lockCtrl.state == ESP32Door::State::showIP) {
        if (servoCtrl.isLocked()) {
          lockCtrl.printLCDPin();
          lockCtrl.state = ESP32Door::State::locked;
        } else {
          lockCtrl.printLCDCenter("Door", "Unlocked");
          lockCtrl.state = ESP32Door::State::unlocked;
        }
      }

      // lock door if currently unlocked
      else if (lockCtrl.state == ESP32Door::State::unlocked || !servoCtrl.isLocked()) {
        lockCtrl.lock();
      }
    }
  }

  prevBtnState = btnState;
  delay(10);
}
