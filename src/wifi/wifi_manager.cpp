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
    // display_show_message("Connexion WiFi...", 0, true); // Appel LCD potentiel

    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
    int timeout_counter = 0;
    const int max_timeout_iterations = WIFI_CONNECTION_TIMEOUT_MS / WIFI_RETRY_DELAY_MS;

    while (WiFi.status() != WL_CONNECTED && timeout_counter < max_timeout_iterations) {
        delay(WIFI_RETRY_DELAY_MS); // Ce delay() est acceptable dans setup()
        Serial.print(".");
        // On pourrait ajouter un indicateur sur le LCD ici aussi (ex: points qui défilent)
        timeout_counter++;
    }
    Serial.println();

    if (wifi_is_connected()) {
        Serial.print("WiFi Connected. IP: ");
        Serial.println(WiFi.localIP());
        // display_show_message("WiFi OK: " + WiFi.localIP().toString(), 0, true); // Appel LCD
    } else {
        Serial.println("WiFi Connection Failed.");
        // display_show_message("Erreur WiFi", 0, true); // Appel LCD
    }
}

bool wifi_is_connected() {
    return (WiFi.status() == WL_CONNECTED);
}