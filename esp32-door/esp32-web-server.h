#ifndef H_WEB_SERVER
#define H_WEB_SERVER

#include <ESPmDNS.h>
#include <WiFi.h>

#include "secrets.h"
#include "esp32-lock-control.h"

#define PIN_LEN 4
#define NFC_LEN 4

namespace ESP32Door {

class LockControl;

class WebServer {
private:
  WiFiServer *server;
  IPAddress localIP;
  String ssid;
  String password;
  String hostname;

public:
  char pin[PIN_LEN + 1];
  byte nfcTag[NFC_LEN + 1];

  WebServer();

  /** @brief Initialises web server to provide HTML page to update PIN */
  void setupWebServer();

  /** @brief Check for any client devices attempting to make requests to the web server */
  void listenClients(LockControl &);

  /** @brief Returns local IPv4 address as string */
  const String getIPString();
};
}

#endif