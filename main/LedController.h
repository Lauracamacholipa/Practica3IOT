#pragma once

#include <Arduino.h>

class LedController {
public:
    LedController(uint8_t redPin, uint8_t yellowPin, uint8_t greenPin);

    void begin();

    void handleCommand(const String& command);

    void setRed();
    void setYellow();
    void setGreen();
    void allOff();

private:
    uint8_t _redPin;
    uint8_t _yellowPin;
    uint8_t _greenPin;

    void _turnAllOff();
};