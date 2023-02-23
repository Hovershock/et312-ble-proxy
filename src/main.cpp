#include <Arduino.h>
#include <HardwareSerial.h>

#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEServer.h>
#include <BLE2902.h>

#include <mk312.h>

#define SERVER_MK312 "C88758C3-413A-48CD-8AB5-9C79DFD98535"

#define CHARACTERISTIC_LEVEL_A "CB7EFDF3-8066-40A5-91B7-8C1E1F498830"
#define CHARACTERISTIC_LEVEL_B "F1AFDE05-D324-49DA-B336-766434634BF3"
#define CHARACTERISTIC_MA "A6C54F45-7F46-4F21-8598-DAF474986B0F"
#define CHARACTERISTIC_MODE "398D747C-0A5E-4678-959C-C696D8147C09"
#define CHARACTERISTIC_ADC_ENABLED "956100EC-12FA-4E62-8B15-EF57649280AB"
#define CHARACTERISTIC_POWER_LEVEL "11565DAB-9C96-41B9-BD72-C6621142FCF3"

void setupBluetooth();

void setup() {
  Serial.begin(115200);
  Serial.println("Delay 10s");

  delay(10000);

  Serial.println("Starting config");

  init_mk312_easy();

  TaskHandle_t bluetoothTaskHandle = NULL;

  setupBluetooth();
  // xTaskCreate(bluetoothTask, "bluetooth", 4096, NULL, 10, &bluetoothTaskHandle);
  Serial.println("Task started");
}

void loop() {
  vTaskDelete(NULL);
}

void setupBluetooth() {
  class ServerCallbacks : public BLEServerCallbacks
  {
        void onConnect(BLEServer *pServer)
        {
            // Uncomment to allow multiple clients
            // pServer->getAdvertising()->start();
            Serial.println("Client connected");
        }

        void onDisconnect(BLEServer *pServer)
        {
            pServer->getAdvertising()->start();
            mk312_all_off();
        }
    };

    class LevelACallbacks : public BLECharacteristicCallbacks
    {
      void onRead(BLECharacteristic *pCharacteristic)
      {
        int levelA = mk312_get_a();
        Serial.printf("Reading level A as %d", levelA);
        pCharacteristic->setValue(levelA);
      }
    };

    BLEDevice::init("Shock");
    BLEServer *pServer = BLEDevice::createServer();
    pServer->setCallbacks(new ServerCallbacks());

    BLEService *pService = pServer->createService(SERVER_MK312);
    BLECharacteristic *levelA = pService->createCharacteristic( CHARACTERISTIC_LEVEL_A, BLECharacteristic::PROPERTY_READ);
    levelA->addDescriptor(new BLE2902());
    levelA->setCallbacks(new LevelACallbacks());

    pService->start();
    BLEAdvertising *pAdvertising = BLEDevice::getAdvertising();
    pAdvertising->addServiceUUID(SERVER_MK312);
    pAdvertising->setScanResponse(true);
    pAdvertising->setMinPreferred(0x06); // functions that help with iPhone connections issue
    pAdvertising->setMinPreferred(0x12);
    BLEDevice::startAdvertising();
}