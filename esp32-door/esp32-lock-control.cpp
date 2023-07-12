#include "esp32-lock-control.h"

namespace ESP32Door {

const char keys[ROW_NUM][COL_NUM] = {
  { '1', '2', '3' },
  { '4', '5', '6' },
  { '7', '8', '9' },
  { '*', '0', '#' }
};

byte pinRows[ROW_NUM] = { 17, 15, 2, 4 };  // follows R1-R4 pins in order
byte pinCols[COL_NUM] = { 16, 5, 0 };      // follows C1-C3 pins in order

LockControl::LockControl() {
  this->state = State::locked;
  this->pinEntry = new char[PIN_LEN];
  this->pinCursor = 0;
}

void LockControl::setWebServer(WebServer *webServer) {
  this->webServer = webServer;
}

void LockControl::setServoControl(ServoControl *servoCtrl) {
  this->servoCtrl = servoCtrl;
}

void LockControl::setNFCReader(NFCReader *nfcReader) {
  this->nfcReader = nfcReader;
}

void LockControl::setupKeypad() {
  keypad = new Keypad(makeKeymap(keys), pinRows, pinCols, ROW_NUM, COL_NUM);
}

char LockControl::readKeypad() {
  return keypad->getKey();
}

void LockControl::readNFC() {
  ESP32Door::NFCReading nfcReading = this->nfcReader->read();
  if (nfcReading.size != 0) {
    bool nfcTagMatch = true;
    for (size_t i = 0; i < NFC_LEN; ++i) {
      if (nfcReading.tag[i] != this->webServer->nfcTag[i])
        nfcTagMatch = false;
    }

    if (nfcTagMatch) {
      // if tag matches, unlock if state is not unlocked OR servo is locked
      if (this->state != State::unlocked || this->servoCtrl->isLocked())
        this->unlock();
    }

    else {
      this->printLCDCenter("Unregistered", "NFC Device");
      delay(2000);
      this->printLCDPin();

      // if tag doesn't match, lock if state is not locked OR servo is unlocked
      if (this->state != State::locked || !this->servoCtrl->isLocked()) {
        this->lock();
      }
    }
  }
}

void LockControl::handlePinEntry(char key) {
  if (this->pinCursor < PIN_LEN) {
    this->pinEntry[this->pinCursor++] = key;
    this->printLCDPin();

    // if PIN length is reached, check its validity
    if (this->pinCursor == PIN_LEN) {
      // if valid, unlock and display unlock message
      if (!strcmp(this->pinEntry, this->webServer->pin)) {
        this->unlock();
      }

      // if invalid, lock if possible and clear input
      else {
        this->printLCDCenter("Wrong", "PIN!");
        delay(2000);
        this->lock();
      }
    }
  }
}

void LockControl::clearPinEntry() {
  for (uint8_t i = 0; i < PIN_LEN; ++i)
    this->pinEntry[i] = '_';
  this->pinEntry[PIN_LEN] = '\0';

  this->pinCursor = 0;
  this->printLCDPin();
}

void LockControl::lock() {
  // only print if it's not locked for some reason
  if (this->state != State::locked) {
    this->printLCDCenter("Locking", "Door...");
  }
  this->servoCtrl->lock();
  this->clearPinEntry();
  this->state = State::locked;
}

void LockControl::unlock() {
  // only print if it's not unlocked for some reason
  if (this->state != State::unlocked) {
    this->printLCDCenter("Unlocking", "Door...");
  }
  this->servoCtrl->unlock();
  this->printLCDCenter("Door", "Unlocked");
  this->state = State::unlocked;
}

void LockControl::setupLCD() {
  this->lcd = new LiquidCrystal_I2C(0x27, 16, 2);
  this->lcd->init();
  this->lcd->backlight();
  this->lcd->clear();
}

void LockControl::printLCDCenter(const char *text) {
  uint8_t padding = getLeftPadding(text);
  this->lcd->clear();
  this->lcd->setCursor(padding, 0);
  this->lcd->print(text);
}

void LockControl::printLCDCenter(const char *text, uint8_t line) {
  uint8_t padding = getLeftPadding(text);
  this->lcd->clear();
  this->lcd->setCursor(padding, line);
  this->lcd->print(text);
}

void LockControl::printLCDCenter(const char *l1, const char *l2) {
  this->lcd->clear();

  uint8_t l1Padding = getLeftPadding(l1);
  this->lcd->setCursor(l1Padding, 0);
  this->lcd->print(l1);

  uint8_t l2Padding = getLeftPadding(l2);
  this->lcd->setCursor(l2Padding, 1);
  this->lcd->print(l2);
}

void LockControl::printLCDPin() {
  this->printLCDCenter("PIN:", this->pinEntry);
}
}

uint8_t getLeftPadding(const char *text) {
  size_t len = strlen(text);
  uint8_t padding = (16 - len) / 2;
  return padding;
}