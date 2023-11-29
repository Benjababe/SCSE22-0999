#ifndef H_PIN_CTRL
#define H_PIN_CTRL

#include <Arduino.h>
#include <LiquidCrystal_I2C.h>
#include <Keypad.h>
#include <UniversalTelegramBot.h>
#include <vector>

#include "esp32-servo.h"
#include "esp32-web-server.h"
#include "esp32-web-client.h"
#include "esp32-nfc.h"
#include "secrets.h"

#define PIN_LEN 4
#define ROW_NUM 4
#define COL_NUM 3

namespace ESP32Door {
enum class State { locked,
                   unlocked,
                   showIP };

class WebServer;

class LockControl {
private:
  WebServer *webServer;
  WebClient *webClient;
  ServoControl *servoCtrl;
  NFCReader *nfcReader;
  Keypad *keypad;
  LiquidCrystal_I2C *lcd;
  char *pinEntry;
  uint8_t pinCursor;

  UniversalTelegramBot *bot;
  unsigned long lastBotHandled;
  int botRequestDelay;
  std::vector<String> acceptedIds;

public:
  /** @brief Maintains the current state of the lock */
  State state;

  LockControl();

  void setWebServer(WebServer *);

  void setWebClient(WebClient *);

  void setServoControl(ServoControl *);

  void setNFCReader(NFCReader *);

  /**  @brief Initialises 4x3 keypad for input */
  void setupKeypad();

  /**  @brief Reads input from the keypad */
  char readKeypad();

  void readNFC();

  /** @brief Logic for handling a keypad press */
  void handlePinEntry(char);

  /** @brief Resets `pinEntry` and `pinCursor` */
  void clearPinEntry();

  /** @brief Resets `pinEntry` and `pinCursor` then locks the door */
  void lock();

  /** @brief Unlocks servo and updates LCD */
  void unlock();

  /** @brief Initialises 16x2 LCD display */
  void setupLCD();

  /** @brief Prints onto both LCD lines with left padding provided for each line */
  void printLCD(const char *, uint8_t, const char *, uint8_t);

  /** @brief Prints onto the top LCD line with auto centering */
  void printLCDCenter(const char *);

  /** @brief Prints onto a selected LCD line with auto centering */
  void printLCDCenter(const char *, uint8_t);

  /** @brief Prints onto both LCD lines with auto centering */
  void printLCDCenter(const char *, const char *);

  /** @brief Prints content of `pinEntry` onto the LCD */
  void printLCDPin();

  /** @brief Listens and handles any new telegram messages */
  void handleNewMessages();

  /** Handles individual new message from user */
  void handleNewMessage(int);

  /** @brief Broadcast a given message to all accepted Ids */
  void broadcastMessage(String);
};
}

/** @brief Calculates padding required to center text on LCD */
uint8_t getLeftPadding(const char *);

/** Helper function to check if element exists in vector `vec` */
template<typename T>
bool vectorContains(std::vector<T> vec, T element) {
  return (std::find(vec.begin(), vec.end(), element) != vec.end());
}

#endif