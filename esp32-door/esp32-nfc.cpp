#include "esp32-nfc.h"

namespace ESP32Door {
NFCReader::NFCReader() {
  SPI.begin();

  this->rfid = new MFRC522(SDA_PIN, RST_PIN);
  this->rfid->PCD_Init();
  Serial.println("RC522 ready");
}

NFCReading NFCReader::read() {
  struct NFCReading nfcReading;

  // Reset the loop if no new card present on the reader. This saves the entire process when idle.
  if (!this->rfid->PICC_IsNewCardPresent())
    return nfcReading;

  // Verify if the NUID has been read
  if (!this->rfid->PICC_ReadCardSerial())
    return nfcReading;

  Serial.print(F("PICC type: "));
  MFRC522::PICC_Type piccType = this->rfid->PICC_GetType(this->rfid->uid.sak);
  Serial.println(this->rfid->PICC_GetTypeName(piccType));

  // Check is the PICC of Classic MIFARE type
  if (piccType != MFRC522::PICC_TYPE_MIFARE_MINI && piccType != MFRC522::PICC_TYPE_MIFARE_1K && piccType != MFRC522::PICC_TYPE_MIFARE_4K) {
    Serial.println(F("Your tag is not of type MIFARE Classic."));
    return nfcReading;
  }

//   Serial.println(F("The UID tag is:"));
//   Serial.print(F("In hex: "));
//   printHex(this->rfid->uid.uidByte, this->rfid->uid.size);
//   Serial.println();

  this->rfid->PICC_HaltA();
  this->rfid->PCD_StopCrypto1();

  nfcReading.tag = this->rfid->uid.uidByte;
  nfcReading.size = this->rfid->uid.size;
  return nfcReading;
}
}

void printHex(byte *buffer, byte bufferSize) {
  for (byte i = 0; i < bufferSize; i++) {
    Serial.print(buffer[i] < 0x10 ? " 0" : " ");
    Serial.print(buffer[i], HEX);
  }
}

void printDec(byte *buffer, byte bufferSize) {
  for (byte i = 0; i < bufferSize; i++) {
    Serial.print(buffer[i] < 0x10 ? " 0" : " ");
    Serial.print(buffer[i], DEC);
  }
}