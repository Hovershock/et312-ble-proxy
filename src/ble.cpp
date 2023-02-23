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

class MACallbacks : public BLECharacteristicCallbacks {
    void onRead(BLECharacteristic *pCharacteristic) {
        // This logic inverts, since my characteristic is if it's enabled or not
        int value = mk312_get_ma();
        pCharacteristic->setValue(value);
        Serial.printf("Reading Level MA as %d\n", value);
    }

    void onWrite(BLECharacteristic *pCharacteristic) {
        int value = readSingle(pCharacteristic->getValue());

        mk312_set_ma(value);
        Serial.printf("Set MA to %d\n", value);
    }
};

class ModeCallbacks : public BLECharacteristicCallbacks {
    void onRead(BLECharacteristic *pCharacteristic) {
        byte value = mk312_get_mode();

        switch (value) {
        case MODE_POWERON:
            pCharacteristic->setValue(STR_MODE_POWERON);
            break;
        case MODE_WAVES:
            pCharacteristic->setValue(STR_MODE_WAVES);
            break;
        case MODE_STROKE:
            pCharacteristic->setValue(STR_MODE_STROKE);
            break;
        case MODE_CLIMB:
            pCharacteristic->setValue(STR_MODE_CLIMB);
            break;
        case MODE_COMBO:
            pCharacteristic->setValue(STR_MODE_COMBO);
            break;
        case MODE_INTENSE:
            pCharacteristic->setValue(STR_MODE_INTENSE);
            break;
        case MODE_RHYTHM:
            pCharacteristic->setValue(STR_MODE_RHYTHM);
            break;
        case MODE_AUDIO1:
            pCharacteristic->setValue(STR_MODE_AUDIO1);
            break;
        case MODE_AUDIO2:
            pCharacteristic->setValue(STR_MODE_AUDIO2);
            break;
        case MODE_AUDIO3:
            pCharacteristic->setValue(STR_MODE_AUDIO3);
            break;
        case MODE_SPLIT:
            pCharacteristic->setValue(STR_MODE_SPLIT);
            break;
        case MODE_RANDOM1:
            pCharacteristic->setValue(STR_MODE_RANDOM1);
            break;
        case MODE_RANDOM2:
            pCharacteristic->setValue(STR_MODE_RANDOM2);
            break;
        case MODE_TOGGLE:
            pCharacteristic->setValue(STR_MODE_TOGGLE);
            break;
        case MODE_ORGASM:
            pCharacteristic->setValue(STR_MODE_ORGASM);
            break;
        case MODE_TORMENT:
            pCharacteristic->setValue(STR_MODE_TORMENT);
            break;
        case MODE_PHASE1:
            pCharacteristic->setValue(STR_MODE_PHASE1);
            break;
        case MODE_PHASE2:
            pCharacteristic->setValue(STR_MODE_PHASE2);
            break;
        case MODE_PHASE3:
            pCharacteristic->setValue(STR_MODE_PHASE3);
            break;
        case MODE_USER1:
            pCharacteristic->setValue(STR_MODE_USER1);
            break;
        case MODE_USER2:
            pCharacteristic->setValue(STR_MODE_USER2);
            break;
        case MODE_USER3:
            pCharacteristic->setValue(STR_MODE_USER3);
            break;
        case MODE_USER4:
            pCharacteristic->setValue(STR_MODE_USER4);
            break;
        case MODE_USER5:
            pCharacteristic->setValue(STR_MODE_USER5);
            break;
        case MODE_USER6:
            pCharacteristic->setValue(STR_MODE_USER6);
            break;
        case MODE_USER7:
            pCharacteristic->setValue(STR_MODE_USER7);
            break;
        default:
            pCharacteristic->setValue(STR_MODE_UNKNOWN);
            break;
        }
    }

    void onWrite(BLECharacteristic *pCharacteristic) {
        const char* value = pCharacteristic->getValue().c_str();

        Serial.printf("Changing mode to %s\n", value);

        // There is probably a better way to do this, but it'll work for now
        if (strcmp(value, STR_MODE_POWERON) == 0) {
            mk312_set_mode(MODE_POWERON);
            return;
        }

        if (strcmp(value, STR_MODE_UNKNOWN) == 0) {
            mk312_set_mode(MODE_UNKNOWN);
            return;
        }

        if (strcmp(value, STR_MODE_WAVES) == 0) {
            mk312_set_mode(MODE_WAVES);
            return;
        }

        if (strcmp(value, STR_MODE_STROKE) == 0) {
            mk312_set_mode(MODE_STROKE);
            return;
        }

        if (strcmp(value, STR_MODE_CLIMB) == 0) {
            mk312_set_mode(MODE_CLIMB);
            return;
        }

        if (strcmp(value, STR_MODE_COMBO) == 0) {
            mk312_set_mode(MODE_COMBO);
            return;
        }

        if (strcmp(value, STR_MODE_INTENSE) == 0) {
            mk312_set_mode(MODE_INTENSE);
            return;
        }

        if (strcmp(value, STR_MODE_RHYTHM) == 0) {
            mk312_set_mode(MODE_RHYTHM);
            return;
        }

        if (strcmp(value, STR_MODE_AUDIO1) == 0) {
            mk312_set_mode(MODE_AUDIO1);
            return;
        }

        if (strcmp(value, STR_MODE_AUDIO2) == 0) {
            mk312_set_mode(MODE_AUDIO2);
            return;
        }

        if (strcmp(value, STR_MODE_AUDIO3) == 0) {
            mk312_set_mode(MODE_AUDIO3);
            return;
        }

        if (strcmp(value, STR_MODE_SPLIT) == 0) {
            mk312_set_mode(MODE_SPLIT);
            return;
        }

        if (strcmp(value, STR_MODE_RANDOM1) == 0) {
            mk312_set_mode(MODE_RANDOM1);
            return;
        }

        if (strcmp(value, STR_MODE_RANDOM2) == 0) {
            mk312_set_mode(MODE_RANDOM2);
            return;
        }

        if (strcmp(value, STR_MODE_TOGGLE) == 0) {
            mk312_set_mode(MODE_TOGGLE);
            return;
        }

        if (strcmp(value, STR_MODE_ORGASM) == 0) {
            mk312_set_mode(MODE_ORGASM);
            return;
        }

        if (strcmp(value, STR_MODE_TORMENT) == 0) {
            mk312_set_mode(MODE_TORMENT);
            return;
        }

        if (strcmp(value, STR_MODE_PHASE1) == 0) {
            mk312_set_mode(MODE_PHASE1);
            return;
        }

        if (strcmp(value, STR_MODE_PHASE2) == 0) {
            mk312_set_mode(MODE_PHASE2);
            return;
        }

        if (strcmp(value, STR_MODE_PHASE3) == 0) {
            mk312_set_mode(MODE_PHASE3);
            return;
        }

        if (strcmp(value, STR_MODE_USER1) == 0) {
            mk312_set_mode(MODE_USER1);
            return;
        }

        if (strcmp(value, STR_MODE_USER2) == 0) {
            mk312_set_mode(MODE_USER2);
            return;
        }

        if (strcmp(value, STR_MODE_USER3) == 0) {
            mk312_set_mode(MODE_USER3);
            return;
        }

        if (strcmp(value, STR_MODE_USER4) == 0) {
            mk312_set_mode(MODE_USER4);
            return;
        }

        if (strcmp(value, STR_MODE_USER5) == 0) {
            mk312_set_mode(MODE_USER5);
            return;
        }

        if (strcmp(value, STR_MODE_USER6) == 0) {
            mk312_set_mode(MODE_USER6);
            return;
        }

        if (strcmp(value, STR_MODE_USER7) == 0) {
            mk312_set_mode(MODE_USER7);
            return;
        }

        Serial.printf("Did not find appropriate mode for %s, not changing\n", value);
    }
};

class PowerLevelCallbacks : public BLECharacteristicCallbacks {
    void onRead(BLECharacteristic *pCharacteristic) {
        byte currentPowerLevel = mk312_get_power_level();

        switch (currentPowerLevel) {
        case POWERLEVEL_LOW:
            pCharacteristic->setValue(STR_POWERLEVEL_LOW);
            break;
        case POWERLEVEL_NORMAL:
            pCharacteristic->setValue(STR_POWERLEVEL_NORMAL);
            break;
        case POWERLEVEL_HIGH:
            pCharacteristic->setValue(STR_POWERLEVEL_HIGH);
            break;
        default:
            Serial.printf("Could not read 0x%02X into a power level\n", currentPowerLevel);
        }
    }

    void onWrite(BLECharacteristic *pCharacteristic) {
        const char* value = pCharacteristic->getValue().c_str();

        if (strcmp(value, STR_POWERLEVEL_LOW) == 0) {
            mk312_set_power_level(POWERLEVEL_LOW);
            return;
        }

        if (strcmp(value, STR_POWERLEVEL_NORMAL) == 0) {
            mk312_set_power_level(POWERLEVEL_NORMAL);
            return;
        }

        if (strcmp(value, STR_POWERLEVEL_HIGH) == 0) {
            mk312_set_power_level(POWERLEVEL_HIGH);
            return;
        }

        Serial.printf("Could not translate %s into power level\n", value);
    }
};

class BatteryLevelCallbacks : public BLECharacteristicCallbacks {
    void onRead(BLECharacteristic *pCharacteristic) {
        int value = mk312_get_battery_level();

        pCharacteristic->setValue(value);
        Serial.printf("Returning battery level as %d\n", value);
    }
};

void setupBluetooth() {
    BLEDevice::init("MK312");
    BLEServer *pServer = BLEDevice::createServer();
    pServer->setCallbacks(new ServerCallbacks());

    BLEService *pService = pServer->createService(BLEUUID(SERVICE_MK312), 30);

    BLECharacteristic *adcEnabled = pService->createCharacteristic(CHARACTERISTIC_ADC_ENABLED, BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_WRITE);

    adcEnabled->addDescriptor(new BLE2902());
    adcEnabled->setCallbacks(new ADCEnabledCallbacks());

    BLECharacteristic *levelA = pService->createCharacteristic(CHARACTERISTIC_LEVEL_A, BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_WRITE);
    levelA->addDescriptor(new BLE2902());
    levelA->setCallbacks(new LevelACallbacks());

    BLECharacteristic *levelB = pService->createCharacteristic(CHARACTERISTIC_LEVEL_B, BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_WRITE);
    levelB->addDescriptor(new BLE2902());
    levelB->setCallbacks(new LevelBCallbacks());

    BLECharacteristic *ma = pService->createCharacteristic(CHARACTERISTIC_MA, BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_WRITE);
    ma->addDescriptor(new BLE2902());
    ma->setCallbacks(new MACallbacks());

    BLECharacteristic *mode = pService->createCharacteristic(CHARACTERISTIC_MODE, BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_WRITE);
    mode->addDescriptor(new BLE2902());
    mode->setCallbacks(new ModeCallbacks());


    BLECharacteristic *powerLevel = pService->createCharacteristic(CHARACTERISTIC_POWER_LEVEL, BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_WRITE);
    powerLevel->addDescriptor(new BLE2902());
    powerLevel->setCallbacks(new PowerLevelCallbacks());

    BLECharacteristic *batteryLevel = pService->createCharacteristic(CHARACTERISTIC_BATTERY_LEVEL, BLECharacteristic::PROPERTY_READ);
    batteryLevel->addDescriptor(new BLE2902());
    batteryLevel->setCallbacks(new BatteryLevelCallbacks());

    pService->start();
    BLEAdvertising *pAdvertising = BLEDevice::getAdvertising();
    pAdvertising->addServiceUUID(SERVICE_MK312);
    pAdvertising->setScanResponse(true);
    pAdvertising->setMinPreferred(0x06); // functions that help with iPhone connections issue
    pAdvertising->setMinPreferred(0x12);
    BLEDevice::startAdvertising();
}
