#include "UltrasonicSensor.h"
#include "config.h"

UltrasonicSensor::UltrasonicSensor(uint8_t trigPin, uint8_t echoPin)
    : _trigPin(trigPin), _echoPin(echoPin), _lastRange(DistanceRange::INVALID) {}

void UltrasonicSensor::begin() {
    pinMode(_trigPin, OUTPUT);
    pinMode(_echoPin, INPUT);
}

float UltrasonicSensor::measure() {
    digitalWrite(_trigPin, LOW);
    delayMicroseconds(2);
    digitalWrite(_trigPin, HIGH);
    delayMicroseconds(10);
    digitalWrite(_trigPin, LOW);

    long duration = pulseIn(_echoPin, HIGH, 50000);

    if (duration == 0) {
        Serial.println("[Sensor] No echo received — measurement discarded");
        return -1;
    }

    float distance = (duration * 0.0343f) / 2.0f;

    if (distance < RANGE_MIN) {
        Serial.printf("[Sensor] Distance %.1f cm below minimum — measurement discarded\n", distance);
        return -1;
    }

    return distance;
}

DistanceRange UltrasonicSensor::classify(float distanceCm) {
    if (distanceCm < 0) return DistanceRange::INVALID;
    if (distanceCm <= RANGE_RED_MAX)    return DistanceRange::RED;
    if (distanceCm <= RANGE_YELLOW_MAX) return DistanceRange::YELLOW;
    if (distanceCm <= RANGE_GREEN_MAX)  return DistanceRange::GREEN;
    return DistanceRange::OUT_OF_RANGE;
}

DistanceRange UltrasonicSensor::getLastRange() const {
    return _lastRange;
}