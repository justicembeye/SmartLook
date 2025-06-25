//
// Created by lozio on 18/05/25.
//


#include "wifi_manager.h"
#include "wifi_config.h"
// #include "../common/globals_config.h"

#include <WiFi.h>
#include <Arduino.h>


void wifi_connect() {
    Serial.print("Connecting to ");
    Serial.println(WIFI_SSID);

    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
    int timeout_counter = 0;
    const int max_timeout_iterations = WIFI_CONNECTION_TIMEOUT_MS / WIFI_RETRY_DELAY_MS;

    while (WiFi.status() != WL_CONNECTED && timeout_counter < max_timeout_iterations) {
        delay(WIFI_RETRY_DELAY_MS);
        Serial.print(".");

        timeout_counter++;
    }
    Serial.println();

    if (wifi_is_connected()) {
        Serial.print("WiFi Connected. IP: ");
        Serial.println(WiFi.localIP());
    } else {
        Serial.println("WiFi Connection Failed.");
    }
}

bool wifi_is_connected() {
    return (WiFi.status() == WL_CONNECTED);
}