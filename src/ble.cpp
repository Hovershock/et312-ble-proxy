#include <Arduino.h>
#include <BLEDevice.h>
#include <BLE2902.h>

#include <mk312.h>

#include "ble.h"

// Read a single byte from a string. Useful for when we aren't, you know, using strings
uint8_t readSingle(std::string s) {
    return s.c_str()[0];
}

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

class ADCEnabledCallbacks : public BLECharacteristicCallbacks {
    void onRead(BLECharacteristic *pCharacteristic) {
        // This logic inverts, since my characteristic is if it's enabled or not
        int value = mk312_get_adc_disabled() ? 0 : 1;
        pCharacteristic->setValue(value);
        Serial.printf("Reading ADC enabled as %d\n", value);
    }

    void onWrite(BLECharacteristic *pCharacteristic) {
        int value = readSingle(pCharacteristic->getValue());

        if (value == 0) {
            mk312_disable_adc();
        } else if (value == 1) {
            mk312_enable_adc();
        } else {
            Serial.printf("Got unknown value for ADC enable: %d\n", value);
        }
    }
};

class LevelACallbacks : public BLECharacteristicCallbacks {
    void onRead(BLECharacteristic *pCharacteristic) {
        // This logic inverts, since my characteristic is if it's enabled or not
        int value = mk312_get_a();
        pCharacteristic->setValue(value);
        Serial.printf("Reading Level A as %d\n", value);
    }

    void onWrite(BLECharacteristic *pCharacteristic) {
        int value = readSingle(pCharacteristic->getValue());

        mk312_set_a(value);
        Serial.printf("Set level A to %d\n", value);
    }
};

class LevelBCallbacks : public BLECharacteristicCallbacks {
    void onRead(BLECharacteristic *pCharacteristic) {
        // This logic inverts, since my characteristic is if it's enabled or not
        int value = mk312_get_b();
        pCharacteristic->setValue(value);
        Serial.printf("Reading Level B as %d\n", value);
    }

    void onWrite(BLECharacteristic *pCharacteristic) {
        int value = readSingle(pCharacteristic->getValue());

        mk312_set_b(value);
        Serial.printf("Set level B to %d\n", value);
    }
};

void setupBluetooth() {
    BLEDevice::init("MK312");
    BLEServer *pServer = BLEDevice::createServer();
    pServer->setCallbacks(new ServerCallbacks());

    BLEService *pService = pServer->createService(SERVICE_MK312);

    BLECharacteristic *adcEnabled = pService->createCharacteristic(CHARACTERISTIC_ADC_ENABLED, BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_WRITE);
    adcEnabled->addDescriptor(new BLE2902());
    adcEnabled->setCallbacks(new ADCEnabledCallbacks());

    BLECharacteristic *levelA = pService->createCharacteristic(CHARACTERISTIC_LEVEL_A, BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_WRITE);
    levelA->addDescriptor(new BLE2902());
    levelA->setCallbacks(new LevelACallbacks());

    BLECharacteristic *levelB = pService->createCharacteristic(CHARACTERISTIC_LEVEL_B, BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_WRITE);
    levelB->addDescriptor(new BLE2902());
    levelB->setCallbacks(new LevelBCallbacks());

    pService->start();
    BLEAdvertising *pAdvertising = BLEDevice::getAdvertising();
    pAdvertising->addServiceUUID(SERVICE_MK312);
    pAdvertising->setScanResponse(true);
    pAdvertising->setMinPreferred(0x06); // functions that help with iPhone connections issue
    pAdvertising->setMinPreferred(0x12);
    BLEDevice::startAdvertising();
}
