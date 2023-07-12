#include "esp32-web-server.h"

namespace ESP32Door {

const long timeoutTimeMs = 2000;

WebServer::WebServer() {
  this->server = new WiFiServer(80);
  this->ssid = "BenjaF3";
  this->password = "benjamin";

  WiFi.setHostname("ESP32-Lock");
  WiFi.begin(ssid.c_str(), password.c_str());

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  for (size_t i = 0; i < PIN_LEN; ++i)
    this->pin[i] = '0';
  this->pin[PIN_LEN] = '\0';

  // init NFC tag values
  this->nfcTag[0] = 0xDA;
  this->nfcTag[1] = 0x6B;
  this->nfcTag[2] = 0x6A;
  this->nfcTag[3] = 0x80;
  this->nfcTag[4] = '\0';

  Serial.println("\nConnected to Wifi!");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  localIP = WiFi.localIP();
  server->begin();
}

void WebServer::listenClients(LockControl &lockCtrl) {
  WiFiClient client = this->server->available();

  if (client) {
    unsigned long currentTime = millis();
    unsigned long previousTime = currentTime;
    String currentLine = "";
    String header = "";

    // provide 2s per client as we still want the CPU to do other things in the meantime
    while (client.connected() && ((currentTime - previousTime) <= timeoutTimeMs)) {
      currentTime = millis();
      if (client.available()) {
        char c = client.read();
        header += c;

        if (c == '\n') {
          if (currentLine.length() == 0) {
            if (header.indexOf("GET /?new_pin=") >= 0) {
              int l = header.indexOf('=');
              int r = header.indexOf('&');
              String newPin = header.substring(l + 1, r);

              for (size_t i = 0; i < PIN_LEN; ++i) {
                this->pin[i] = newPin[i];
              }

              // print on lcd on PIN change
              lockCtrl.printLCDCenter("PIN changed to:", this->pin);
              delay(5000);
              lockCtrl.lock();
            }

            // send to client html page to update PIN
            client.println("HTTP/1.1 200 OK");
            client.println("Content-type:text/html");
            client.println("Connection: close");
            client.println();

            client.println("<!DOCTYPE html><html>");
            client.println("<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">");
            client.println("<link rel=\"icon\" href=\"data:,\">");

            client.println("<script>function changePin() {");
            client.println("let newPin = document.querySelector(\"#txtPin\").value;");
            client.println("while (newPin.length < " + String(PIN_LEN) + "){ newPin += '0' }");
            client.println("if (newPin.length > " + String(PIN_LEN) + ") { newPin = newPin.substring(0, " + String(PIN_LEN) + ") }");
            client.println("window.location.href=`/?new_pin=${newPin}`");
            client.println("}</script>");

            client.println("</head><body><h1>ESP32 Lock</h1>");
            client.println("<span>PIN:</span>");
            client.println("<input type=\"text\" id=\"txtPin\" value=\"" + String(pin) + "\"/>");
            client.println("<button onclick=\"changePin()\">Change PIN</button>");
            client.println("</body></html>");
            break;
          } else {
            currentLine = "";
          }
        } else if (c != '\r') {
          currentLine += c;
        }
      }
    }
    client.stop();
  }
}

const String WebServer::getIPString() {
  return this->localIP.toString();
}

}