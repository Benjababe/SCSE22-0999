#ifndef H_NFC
#define H_NFC

#include <MFRC522.h>

// Other pins
// SCK      GPIO18
// MISO     GPIO19
// MOSI     GPIO23
#define RST_PIN 32
#define SDA_PIN 33

namespace ESP32Door {

struct NFCReading {
  byte size;
  byte* tag;
};

class NFCReader {
private:
  MFRC522 *rfid;
public:
  NFCReader();

  /** @brief Reads RC522 for any NFC object */
  NFCReading read();
};
}

/** @brief Helper function to print buffer in hexadecimal */
void printHex(byte *buffer, byte bufferSize);

/** @brief Helper function to print buffer in decimal */
void printDec(byte *buffer, byte bufferSize);

#endif