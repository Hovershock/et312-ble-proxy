#include <Arduino.h>
#include <HardwareSerial.h>

#include <mk312.h>
#include "ble.h"

void setup() {
  Serial.begin(115200);
  // Serial.println("Delay 10s");

  // delay(10000);

  Serial.println("Starting config");

  init_mk312_easy();

  setupBluetooth();
  Serial.println("Setup finished");
}

void loop() {
  vTaskDelete(NULL);
}