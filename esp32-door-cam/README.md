# ESP32 Door Camera

## Setup

Apart from external libraries, a `secrets.h` file is required in the project directory containing the following

```c++
#ifndef H_SECRET
#define H_SECRET

#include <pgmspace.h>

// Wifi SSID
#define SECRET_SSID ""

// Wifi password
#define SECRET_PASSWORD ""

// Thing name on AWS IoT Core
#define SECRET_AWS_IOT_CLIENT_ID ""

// Endpoint for AWS IoT Core Thing
#define SECRET_AWS_IOT_ENDPOINT ""

// Bot token for the home bot
#define SECRET_TELEGRAM_BOT_TOKEN "xxxxx:xxxxx"

// Vector of telegram ids for the bot to broadcast to
#define SECRET_TELEGRAM_IDS { "", "" }

// Amazon Root CA 1
static const char SECRET_AWS_CERT_CA[] PROGMEM = R"EOF(
-----BEGIN CERTIFICATE-----
-----END CERTIFICATE-----
)EOF";

// Device Certificate
static const char SECRET_AWS_CERT_CRT[] PROGMEM = R"KEY(
-----BEGIN CERTIFICATE-----
-----END CERTIFICATE-----
)KEY";

// Device Private Key
static const char SECRET_AWS_CERT_PRIVATE[] PROGMEM = R"KEY(
-----BEGIN RSA PRIVATE KEY-----
-----END RSA PRIVATE KEY-----
)KEY";
```
