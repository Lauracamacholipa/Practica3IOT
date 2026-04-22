#include "MQTTManager.h"
#include "config.h"

MQTTManager* MQTTManager::_instance = nullptr;

MQTTManager::MQTTManager(const char* ssid, const char* password,
                         const char* broker, uint16_t port,
                         const char* clientId)
    : _ssid(ssid), _password(password),
      _broker(broker), _port(port), _clientId(clientId),
      _mqttClient(_wifiClient), _lastReconnectAttempt(0) {
    _instance = this;
}

void MQTTManager::setMessageCallback(MessageCallback callback) {
    _userCallback = callback;
}

void MQTTManager::begin() {
    _connectWiFi();
    _wifiClient.setInsecure(); // Disable SSL certificate verification
    _mqttClient.setServer(_broker, _port);
    _mqttClient.setCallback(_onMessage);
    _connectMQTT();
}

void MQTTManager::subscribe(const char* topic) {
    _mqttClient.subscribe(topic);
    Serial.printf("[MQTT] Subscribed to: %s\n", topic);
}

void MQTTManager::publish(const char* topic, const String& payload) {
    if (_mqttClient.publish(topic, payload.c_str())) {
        Serial.printf("[MQTT] Published to %s: %s\n", topic, payload.c_str());
    } else {
        Serial.printf("[MQTT] Publish failed to %s\n", topic);
    }
}

void MQTTManager::loop() {
    if (!_mqttClient.connected()) {
        unsigned long now = millis();
        if (now - _lastReconnectAttempt >= RECONNECT_INTERVAL) {
            _lastReconnectAttempt = now;
            Serial.println("[MQTT] Connection lost — attempting reconnect...");
            _connectMQTT();
        }
    }
    _mqttClient.loop();
}

bool MQTTManager::isConnected() {
    return _mqttClient.connected();
}

void MQTTManager::_connectWiFi() {
    Serial.printf("[WiFi] Connecting to %s", _ssid);
    WiFi.begin(_ssid, _password);
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }
    Serial.printf("\n[WiFi] Connected. IP: %s\n", WiFi.localIP().toString().c_str());
}

void MQTTManager::_connectMQTT() {
    String clientId = String(_clientId) + "-" + String(random(10000));

    if (_mqttClient.connect(clientId.c_str(), MQTT_USER, MQTT_PASS)){
        Serial.printf("[MQTT] Connected as %s\n", _clientId);
        subscribe(TOPIC_COMMANDS);
    } else {
        Serial.printf("[MQTT] Connection failed. State: %d\n", _mqttClient.state());
        Serial.println("[MQTT] Check TLS / credentials / clientId");
    }
}

void MQTTManager::_onMessage(char* topic, byte* payload, unsigned int length) {
    if (_instance == nullptr || !_instance->_userCallback) return;

    String topicStr(topic);
    String payloadStr;
    for (unsigned int i = 0; i < length; i++) {
        payloadStr += (char)payload[i];
    }

    Serial.printf("[MQTT] Message received on %s: %s\n", topic, payloadStr.c_str());
    _instance->_userCallback(topicStr, payloadStr);
}