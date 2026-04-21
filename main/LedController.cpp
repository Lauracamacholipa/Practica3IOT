#include "LedController.h"

LedController::LedController(uint8_t redPin, uint8_t yellowPin, uint8_t greenPin)
    : _redPin(redPin), _yellowPin(yellowPin), _greenPin(greenPin) {}

void LedController::begin() {
    pinMode(_redPin,    OUTPUT);
    pinMode(_yellowPin, OUTPUT);
    pinMode(_greenPin,  OUTPUT);
    _turnAllOff();
}

void LedController::handleCommand(const String& command) {
    unsigned long start = millis();

    if (command == "LED_RED_ON") {
        setRed();
    } else if (command == "LED_YELLOW_ON") {
        setYellow();
    } else if (command == "LED_GREEN_ON") {
        setGreen();
    } else if (command == "ALL_OFF") {
        allOff();
    } else {
        Serial.printf("[LED] Unknown command: %s\n", command.c_str());
        return;
    }

    Serial.printf("[LED] Command '%s' processed in %lu ms\n", command.c_str(), millis() - start);
}

void LedController::setRed() {
    _turnAllOff();
    digitalWrite(_redPin, HIGH);
    Serial.println("[LED] RED on");
}

void LedController::setYellow() {
    _turnAllOff();
    digitalWrite(_yellowPin, HIGH);
    Serial.println("[LED] YELLOW on");
}

void LedController::setGreen() {
    _turnAllOff();
    digitalWrite(_greenPin, HIGH);
    Serial.println("[LED] GREEN on");
}

void LedController::allOff() {
    _turnAllOff();
    Serial.println("[LED] All off");
}

void LedController::_turnAllOff() {
    digitalWrite(_redPin,    LOW);
    digitalWrite(_yellowPin, LOW);
    digitalWrite(_greenPin,  LOW);
}