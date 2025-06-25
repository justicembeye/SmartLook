//
// Created by lozio on 26/05/25.
//

#ifndef APP_CONFIG_H
#define APP_CONFIG_H

#include <stdint.h>

struct ApplicationSettings {
    uint8_t max_failed_attempts;
    int code_validity_seconds;

    // Valori predefiniti usati se il recupero tramite API fallisce o prima del recupero
    ApplicationSettings() : max_failed_attempts(3), code_validity_seconds(300) {}
};

// Istanza globale dei parametri dell'applicazione.
// Verrà definita in app_config.cpp
extern ApplicationSettings app_settings;

// Funzione per caricare i parametri dall'API.
// Deve essere chiamata dopo la connessione WiFi.
void load_app_settings_from_api();

#endif //APP_CONFIG_H
