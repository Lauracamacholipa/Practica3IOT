#include "config.h"
#include "UltrasonicSensor.h"
#include "LedController.h"
#include "MQTTManager.h"

UltrasonicSensor sensor(PIN_TRIG, PIN_ECHO);
LedController    leds(PIN_LED_RED, PIN_LED_YELLOW, PIN_LED_GREEN);
MQTTManager      mqtt(WIFI_SSID, WIFI_PASSWORD, MQTT_BROKER, MQTT_PORT, MQTT_CLIENT_ID);

DistanceRange lastPublishedRange = DistanceRange::INVALID;

void onMqttMessage(const String& topic, const String& payload) {
    if (topic == TOPIC_COMMANDS) {
        leds.handleCommand(payload);
    }
}

void setup() {
    Serial.begin(115200);

    sensor.begin();
    leds.begin();

    mqtt.setMessageCallback(onMqttMessage);
    mqtt.begin();
}

void loop() {
    mqtt.loop();

    float distance = sensor.measure();

    if (distance < 0) {
        delay(200);
        return;
    }

    DistanceRange currentRange = sensor.classify(distance);

    if (currentRange != lastPublishedRange) {
        lastPublishedRange = currentRange;

        String payload = String(distance, 1); // one decimal place
        mqtt.publish(TOPIC_SENSOR, payload);

        Serial.printf("[Main] Range changed — distance: %.1f cm published\n", distance);
    }

    delay(200);
}
