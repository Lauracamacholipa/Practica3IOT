#pragma once
 
#define WIFI_SSID     "Say My Name"
#define WIFI_PASSWORD "h3is3nb3rg"
 
#define MQTT_BROKER   "2a343cb26d7749cba23d29071060df5a.s1.eu.hivemq.cloud"
#define MQTT_PORT     8883
#define MQTT_CLIENT_ID "esp32-grupo3"

#define MQTT_USER      "grupo3"
#define MQTT_PASS      "Grupo3000"
 
#define TOPIC_SENSOR   "sis234/grupo3/sensor/distance"
#define TOPIC_COMMANDS "sis234/grupo3/actuator/leds"
 
#define PIN_TRIG 26
#define PIN_ECHO 25
 
#define PIN_LED_RED    5
#define PIN_LED_YELLOW 17
#define PIN_LED_GREEN  16
 
#define RANGE_MIN       2
#define RANGE_RED_MAX   30
#define RANGE_YELLOW_MAX 100
#define RANGE_GREEN_MAX  150
 
#define RECONNECT_INTERVAL 2000