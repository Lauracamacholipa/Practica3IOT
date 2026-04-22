#pragma once

#include <Arduino.h>

// Distance ranges for classification
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

    // Returns measured distance in cm; returns -1 if invalid
    float measure();

    // Classifies a distance value into a DistanceRange
    DistanceRange classify(float distanceCm);

    // Returns the last valid range detected
    DistanceRange getLastRange() const;

private:
    uint8_t _trigPin;
    uint8_t _echoPin;
    DistanceRange _lastRange;
};