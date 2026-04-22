#pragma once

#include <Arduino.h>
#include <WiFiClientSecure.h>
#include <WiFi.h>
#include <PubSubClient.h>

using MessageCallback = std::function<void(const String& topic, const String& payload)>;

class MQTTManager {
public:
    MQTTManager(const char* ssid, const char* password,
                const char* broker, uint16_t port,
                const char* clientId);

    void setMessageCallback(MessageCallback callback);

    void begin();

    void subscribe(const char* topic);

    void publish(const char* topic, const String& payload);

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

    static void _onMessage(char* topic, byte* payload, unsigned int length);

    static MQTTManager* _instance;
};