#pragma once

#include <Arduino.h>

class LedController {
public:
    LedController(uint8_t redPin, uint8_t yellowPin, uint8_t greenPin);

    void begin();

    // Handles MQTT command strings
    // Valid commands: LED_RED_ON, LED_YELLOW_ON, LED_GREEN_ON, ALL_OFF
    void handleCommand(const String& command);

    // Activates a single LED and turns the others off
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