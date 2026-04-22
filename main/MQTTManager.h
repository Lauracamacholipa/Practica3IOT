#pragma once

#include <Arduino.h>
#include <WiFiClientSecure.h>
#include <WiFi.h>
#include <PubSubClient.h>

// Callback type for incoming MQTT messages
using MessageCallback = std::function<void(const String& topic, const String& payload)>;

class MQTTManager {
public:
    MQTTManager(const char* ssid, const char* password,
                const char* broker, uint16_t port,
                const char* clientId);

    // Sets the function to call when a message arrives
    void setMessageCallback(MessageCallback callback);

    // Connects to WiFi and then to the MQTT broker
    void begin();

    // Subscribes to a topic
    void subscribe(const char* topic);

    // Publishes a message to a topic
    void publish(const char* topic, const String& payload);

    // Must be called in loop() — handles reconnection and incoming messages
    void loop();

    bool isConnected();

private:
    const char* _ssid;
    const char* _password;
    const char* _broker;
    uint16_t    _port;
    const char* _clientId;

    WiFiClientSecure _wifiClient;
    PubSubClient _mqttClient;

    MessageCallback _userCallback;

    unsigned long _lastReconnectAttempt;

    void _connectWiFi();
    void _connectMQTT();

    // Internal PubSubClient callback — forwards to _userCallback
    static void _onMessage(char* topic, byte* payload, unsigned int length);

    // Pointer to the singleton instance for static callback
    static MQTTManager* _instance;
};