#include <Arduino.h>

enum PowerLevel
{
    POWER_LEVEL_LOW = 0x01,
    POWER_LEVEL_NORMAL = 0x02,
    POWER_LEVEL_HIGH = 0x03
};

enum Button
{
    BUTTON_MENU = 0x80,
    BUTTON_OK = 0x20,
    BUTTON_LEFT = 0x10,
    BUTTON_RIGHT = 0x40
};

enum Modes
{
    MODE_POWERON = 0x00,
    MODE_UNKNOWN = 0x01,
    MODE_WAVES = 0x76,
    MODE_STROKE = 0x77,
    MODE_CLIMB = 0x78,
    MODE_COMBO = 0x79,
    MODE_INTENSE = 0x7a,
    MODE_RHYTHM = 0x7b,
    MODE_AUDIO1 = 0x7c,
    MODE_AUDIO2 = 0x7d,
    MODE_AUDIO3 = 0x7e,
    MODE_SPLIT = 0x7f,
    MODE_RANDOM1 = 0x80,
    MODE_RANDOM2 = 0x81,
    MODE_TOGGLE = 0x82,
    MODE_ORGASM = 0x83,
    MODE_TORMENT = 0x84,
    MODE_PHASE1 = 0x85,
    MODE_PHASE2 = 0x86,
    MODE_PHASE3 = 0x87,
    MODE_USER1 = 0x88,
    MODE_USER2 = 0x89,
    MODE_USER3 = 0x8a,
    MODE_USER4 = 0x8b,
    MODE_USER5 = 0x8c,
    MODE_USER6 = 0x8d,
    MODE_USER7 = 0x8e
};

class ET312
{
private:
    Stream *_serial;
    Stream *_debug;
    bool _enableDebug = false;
    bool _didHandshake = false;
    uint8_t _key = 0x00;

    SemaphoreHandle_t _serialMutex;
    SemaphoreHandle_t _debugMutex;

    bool readAddress(uint16_t address, uint8_t* data);
    bool writeAddress(uint16_t address, uint8_t* data, size_t length);
    bool handshake();
    
    bool readUntil(uint8_t desiredByte, uint8_t* buf, size_t len, uint maximumRetries);
    uint32_t flushReadBuffer();

    void debug(const char* fmt, ...);
public:
    ET312(Stream &serial, Stream &debug);
    ET312(Stream &serial);

    bool keyExchange();
    bool setKey(uint8_t key);
    bool pushButton(Button button);
    bool showMainMenu();
    bool close(); // Close the connection and reset our key

    bool setMode(Modes mode);
    bool setLevelA(uint8_t levelA);
    bool setLevelB(uint8_t levelB);
    bool setLevelMA(uint8_t level);
    bool setPowerLevel(PowerLevel powerLevel);
    bool setADCEnabled(bool enabled);

    bool getMode(Modes *mode);
    bool getLevelA(uint8_t *levelA);
    bool getLevelB(uint8_t *levelB);
    bool getLevelMA(uint8_t *level);
    bool getMARange(uint8_t *low, uint8_t *high);
    bool getPowerLevel(PowerLevel *powerLevel);
    bool getADCEnabled(bool *enabled);
    bool getOnWallPower(bool *onWallPower);
    bool getBatteryLevel(uint8_t *level);
};
