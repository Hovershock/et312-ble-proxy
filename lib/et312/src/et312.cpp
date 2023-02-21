#include "et312.h"

#define MAGIC_BYTE_KEY 0x55 // The magic key used for encryption
#define DEBUG_PRINT_BUFFER_SIZE 256

//Memory addresses
#define ADDRESS_R15 0x400f
#define ADDRESS_ADC_POWER 0x4062
#define ADDRESS_ADC_BATTERY 0x4063
#define ADDRESS_LEVELA 0x4064
#define ADDRESS_LEVELB 0x4065
#define ADDRESS_PUSH_BUTTON 0x4068
#define ADDRESS_COMMAND_1 0x4070
#define ADDRESS_COMMAND_2 0x4071
#define ADDRESS_MA_MIN_VALUE 0x4086
#define ADDRESS_MA_MAX_VALUE 0x4087
#define ADDRESS_CURRENT_MODE 0x407b
#define ADDRESS_LCD_WRITE_PARAMETER_1 0x4180
#define ADDRESS_LCD_WRITE_PARAMETER_2 0x4181
#define ADDRESS_POWER_LEVEL 0x41f4
#define ADDRESS_BATTERY_LEVEL 0x4203
#define ADDRESS_LEVELMA 0x420D
#define ADDRESS_KEY 0x4213

bool verifyChecksum(uint8_t* data, size_t len);

ET312::ET312(Stream &serial, Stream &debug) {
    _serial = &serial;
    _debug = &debug;
    _enableDebug = true;

    // TODO: Make sure this was created property
    _serialMutex = xSemaphoreCreateMutex();
    _debugMutex = xSemaphoreCreateMutex();
}

ET312::ET312(Stream &serial) {
    _serial = &serial;

    // TODO: Make sure this was created property
    _serialMutex = xSemaphoreCreateMutex(); 
}

bool ET312::keyExchange() {
    if (!xSemaphoreTake(_serialMutex, portTICK_RATE_MS * 10))
    {
        debug("Unable to get semaphore during handshake");
        return false;
    }

    _serial -> setTimeout(100);

    size_t writeCount;
    size_t readCount;
    uint8_t buffer[4];

    if(!handshake()) {
        debug("Could not execute initial handshake during key exchange");
        xSemaphoreGive(_serialMutex);
        return false;
    }

    // Set the host key to 0x00. Note we aren't using readBytes
    // and writeBytes because handshaking is special
    buffer[0] = 0x2f;
    buffer[1] = 0x00;
    buffer[2] = 0x2f;

    debug("Sending key exchange request: 0x%02x 0x%02x 0x%02x", buffer[0], buffer[1], buffer[2]);

    // Send the init command
    writeCount = _serial->write(buffer, 3);
    if (writeCount != 3) {
        debug("Wrote %d bytes when sending initial key exchange", writeCount);
        xSemaphoreGive(_serialMutex);
        return false;
    }

    if (!readUntil(0x21, buffer, 3, 32)) {
        debug("Could not find 0x21 during key exchange");
        xSemaphoreGive(_serialMutex);
        return false;
    }

    if (!verifyChecksum(buffer, 3)) {
        debug("Got invalid checksum when reading key exchange: 0x%02x 0x%02x 0x%02x", buffer[0], buffer[1], buffer[2]);
        xSemaphoreGive(_serialMutex);
        return false;
    }

    if (buffer[0] != 0x21) {
        debug("Got 0x%02x as first byte during key exchange", buffer[0]);
        xSemaphoreGive(_serialMutex);
        return false;
    }

    uint8_t boxKey = buffer[1];

    // The host key is always 0x00, so we can skip a bunch of the calculations there
    // The box key *should* always be 0x00 for the patched firmware, but just in case
    // we are reading it and storing it
    _key = boxKey ^ MAGIC_BYTE_KEY;

    debug("Set new encryption key: 0x%02x, handshaking again", _key);
    if (!handshake()) {
        debug("Did not handshake after key exchange");
        xSemaphoreGive(_serialMutex);
        return false;
    }

    return true;
}

uint32_t ET312::flushReadBuffer() {
    uint32_t count = 0;

    while (_serial->available()){
        int data = _serial->read();
        count ++;
        debug("Got data during buffer flush: 0x%02x", data);
    }

    return count;
}

// Send a 0x00 and expect a 0x07, clear the read buffers
bool ET312::handshake() {
    flushReadBuffer();
    _serial -> setTimeout(100);

    uint8_t buffer[1];

    for (int i = 0; i < 16; i++) {
        flushReadBuffer();

        buffer[0] = 0x00 ^ _key;
        debug("Sending 0x%02x for handshake", buffer[0]);

        size_t writeCount = _serial -> write(buffer, 1);
        if (writeCount != 1) {
            debug("Wrote %d bytes when sending handshake packet", writeCount);
            continue;
        }

        if (!readUntil(0x07, buffer, 1, 32)) {
            debug("Did not get 0x07 during handshake: 0x%02x", buffer[0]);
            continue;
        }

        debug("Handshake OK");

        xSemaphoreGive(_serialMutex);
        return true;
    }

    debug("Ran out of retries during handshake");
    xSemaphoreGive(_serialMutex);
    return false;
}

bool ET312::setKey(uint8_t key) {
    _key = key;

    return handshake();
}

bool ET312::readAddress(uint16_t address, uint8_t* data) {
    if (!xSemaphoreTake(_serialMutex, portTICK_RATE_MS * 10))
    {
        debug("Unable to get semaphore during address read");
        return false;
    }

    _serial -> setTimeout(100);

    // // This will make sure we are connected and flush any crap out of the buffers
    // if (!handshake()) {
    //     debug("Could not execute handshake during address read");
    //     xSemaphoreGive(_serialMutex);
    //     return false;
    // }

    flushReadBuffer();

    uint8_t buffer[4];

    
    buffer[0] = 0x3c; // Read address command
    buffer[1] = (address >> 8) & 0xFF; // Low address
    buffer[2] = address & 0xFF; // High address
    buffer[3] = buffer[0] + buffer[1] + buffer[2]; // Checksum

    // Apply box encryption
    for(int i = 0; i < 4; i++) {
        buffer[0] = buffer[0] ^ _key;
    }

    debug("Sending address request: 0x%02x 0x%02x 0x%02x 0x%02x", buffer[0], buffer[1], buffer[2], buffer[3]);

    // Write the request out
    size_t writeCount = _serial -> write(buffer, 4);
    if (writeCount != 4) {
        debug("Wrote %d bytes during address read operation", writeCount);
        xSemaphoreGive(_serialMutex);
        return false;
    }

    // Read 3 bytes back in with the response
    size_t readCount = _serial -> readBytes(buffer, 3);
    if (readCount != 3) {
        debug("Read %d bytes during address read operation", writeCount);
        xSemaphoreGive(_serialMutex);
        return false;
    }

    if (!verifyChecksum(buffer, 3)) {
        debug("Checksum did not verify during address read: 0x%02x 0x%02x 0x%02x", buffer[0], buffer[1], buffer[2]);
        xSemaphoreGive(_serialMutex);
        return false;
    }

    if (buffer[0] != 0x22) {
        debug("Did not get 0x22 back during address read response: 0x%02x", buffer[0]);
        xSemaphoreGive(_serialMutex);
        return false;
    }

    *data = buffer[1];
    return true;
}

bool ET312::writeAddress(uint16_t address, uint8_t* data, size_t len) {
    if (len > 8) {
        debug("Cannot write more than 8 bytes to the ET312. Got %d", len);
        return false;
    }

    if (!xSemaphoreTake(_serialMutex, portTICK_RATE_MS * 10))
    {
        debug("Unable to get semaphore during address read");
        return false;
    }

    _serial -> setTimeout(100);
    
    // This will make sure we are connected and flush any crap out of the buffers
    if (!handshake()) {
        debug("Could not execute handshake during address read");
        xSemaphoreGive(_serialMutex);
        return false;
    }

    uint8_t buffer[4 + len];

    // Basic header for data write
    buffer[0] = 0x03 + (len << 4);
    buffer[1] = (address >> 8) & 0xff;
    buffer[2] = address & 0xff;
    
    // Copy the data into the buffer
    for (size_t i = 0; i < len; i++){
        buffer[i+3] = data[i];
    }

    // Calculate the checksum
    uint8_t checksum = 0;
    for (size_t i = 0; i < len + 3; i++) {
        checksum += buffer[i];
    }

    // The mod here should be redundant, since the 8 bit number should be wrapping,
    // but I like making the algorithm clearer
    buffer[3 + len] = checksum % 0x100;

    size_t writeCount = _serial -> write(buffer, 4 + len);
    if (writeCount != 4 + len) {
        debug("Did not write %d bytes during memory wrote. Wrote %d instead", 4+len, writeCount);
        xSemaphoreGive(_serialMutex);
        return false;
    }

    // Read the acknowledgement back out
    size_t readCount = _serial -> readBytes(buffer, 1);
    if (readCount != 1) {
        debug("Read %d bytes during serial write check", readCount);
        xSemaphoreGive(_serialMutex);
        return false;
    }

    if (buffer[0] != 0x06) {
        debug("Got back 0x%02x during serial write check", buffer[0]);
        xSemaphoreGive(_serialMutex);
        return false;
    }

    return true;
}

bool verifyChecksum(uint8_t* data, size_t len) {
    uint8_t sum = 0x00;

    for (int i = 0; i < len-1; i++) {
        sum += data[i];
    }

    // This should be a noop, since it should have overflowed, but just in case
    uint8_t expected = sum % 0x100;
    uint8_t actual = data[len-1];
    return actual == expected;
}

void ET312::debug(const char* fmt, ...) {
    va_list args;
    va_start(args, fmt);
    // TODO: This will have to fit into each task's stack. Do something to confirm we have enough stack left
    char out[DEBUG_PRINT_BUFFER_SIZE];
    vsnprintf(out, DEBUG_PRINT_BUFFER_SIZE, fmt, args);
    va_end(args);

    if (xSemaphoreTake(_debugMutex, portMAX_DELAY)) {
        // This really should never fail

        Serial.println(out);
        xSemaphoreGive(_debugMutex);
    }
}

bool ET312::getLevelA(uint8_t* data) {
    return readAddress(ADDRESS_LEVELA, data);
}

// Read until we get our desired byte
bool ET312::readUntil(uint8_t desiredByte, uint8_t* buf, size_t len, uint maximumRetries) {
    // Read in 1 byte chunks until we get what we want 

    size_t readSize;
    uint retries = 0;
    while(true) {
        readSize = _serial->readBytes(buf, 1);

        if (readSize != 1) {
            debug("Did not get any data when looking for 0x%02x", desiredByte);
            return false;
        }

        debug("Got byte 0x%02x when searching for 0x%02x", buf[0], desiredByte);
        if (buf[0] == desiredByte)
            break;
        
        retries++;
        if (retries > maximumRetries) {
            debug("Ran out of retries");
            return false;
        }

    }

    // We've gotten our desired byte and it's now in buffer position 0

    // If this is all we were looking for... we're done
    if (len == 1)
        return true;
    
    // Read one fewer byte, starting one additional byte in
    // since our desired byte is already in position 0
    readSize = _serial->readBytes(buf + 1, len - 1);

    if (readSize != len - 1) {
        debug("Got %d bytes when reading, expected %d", readSize, len - 1);
        return false;
    }

    debug("Read was successful");
    return true;
}