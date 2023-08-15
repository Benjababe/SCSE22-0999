#define CAMERA_MODEL_AI_THINKER

#include <esp_camera.h>
#include <ESPmDNS.h>
#include "FS.h"
#include "SD_MMC.h"
#include <WiFi.h>

#include "camera_pins.h"
#include "camera_code.h"
#include "secrets.h"
#include "esp32-web-client.h"

const char* hostname = "esp32-door-cam";
WebClient webClient;

void startCameraServer();

void setup() {
  WRITE_PERI_REG(RTC_CNTL_BROWN_OUT_REG, 0);

  Serial.begin(115200);
  Serial.setDebugOutput(true);
  Serial.println();

  Serial.println("Starting SD Card");
  if(!SD_MMC.begin()){
    Serial.println("SD Card Mount Failed");
    return;
  }
  
  uint8_t cardType = SD_MMC.cardType();
  if(cardType == CARD_NONE){
    Serial.println("No SD Card attached");
    return;
  }

  setupCamera();

  WiFi.mode(WIFI_STA);
  WiFi.setHostname(hostname);
  WiFi.begin(SECRET_SSID, SECRET_PASSWORD);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");

  if (!MDNS.begin(hostname)) {
    Serial.println("Error starting mDNS");
  }

  Serial.print("Camera Ready! Use 'http://");
  Serial.print(WiFi.localIP());
  Serial.println("' to connect");

  MDNS.addService("http", "tcp", 80);

  webClient.connectAWS();
}

void loop() {
  webClient.refreshConnection();
  delay(1000);
}
