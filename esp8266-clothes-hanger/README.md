# ESP32 Door Camera

## Setup

Apart from external libraries, a `secrets.h` file is required in the project directory containing the following

```c++
#ifndef H_SECRET
#define H_SECRET

// Wifi SSID
#define SECRET_SSID ""

// Wifi password
#define SECRET_PASSWORD ""

// Bot token for the home bot
#define SECRET_TELEGRAM_BOT_TOKEN "xxxxx:xxxxx"

#endif
```
