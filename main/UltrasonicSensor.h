#pragma once

#include <Arduino.h>

enum class DistanceRange {
    INVALID,   // out of sensor bounds or no echo
    RED,       // 2–30 cm
    YELLOW,    // 31–100 cm
    GREEN,     // 101–150 cm
    OUT_OF_RANGE // > 150 cm
};

class UltrasonicSensor {
public:
    UltrasonicSensor(uint8_t trigPin, uint8_t echoPin);

    void begin();

    float measure();

    DistanceRange classify(float distanceCm);

    DistanceRange getLastRange() const;

private:
    uint8_t _trigPin;
    uint8_t _echoPin;
    DistanceRange _lastRange;
};