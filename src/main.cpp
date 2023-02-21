#include <Arduino.h>
#include <HardwareSerial.h>

#include "et312.h"

HardwareSerial BoxSerial(2);
ET312 et312 = ET312(BoxSerial, Serial);

void setup() {
  Serial.begin(115200);
  BoxSerial.begin(19200, SERIAL_8N1, 6, 5);

  // Serial.println("Beginning key exchange");
  // bool keyExchangeSuccessful = et312.keyExchange();
  // if (keyExchangeSuccessful) {
  //   Serial.println("Key exchange was successful");
  // } else {
  //   Serial.println("Key exchange was not successful");
  // }
}

void loop() {
  uint8_t levelA;
  if (et312.getLevelA(&levelA)) {
    Serial.printf("Level A: %d\n", levelA);
  } else {
    Serial.println("Could not read level A");
  }

  delay(2000);
  // Serial.printf("Key exchange: %s\n", keyExchangeSuccessful);
  
  // put your main code here, to run repeatedly:
}