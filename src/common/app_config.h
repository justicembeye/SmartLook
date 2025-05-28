//
// Created by lozio on 26/05/25.
//

#ifndef APP_CONFIG_H
#define APP_CONFIG_H

#include <stdint.h> // Pour uint8_t

struct ApplicationSettings {
    uint8_t max_failed_attempts;
    int code_validity_seconds;
    // uint8_t code_length; // Décommentez si vous gérez aussi la longueur du code

    // Valeurs par défaut utilisées si la récupération API échoue ou avant la récupération
    ApplicationSettings() : max_failed_attempts(3), code_validity_seconds(300) {}
};

// Instance globale des paramètres de l'application.
// Elle sera définie dans app_config.cpp
extern ApplicationSettings app_settings;

// Fonction pour charger les paramètres depuis l'API.
// Doit être appelée après la connexion WiFi.
void load_app_settings_from_api();

#endif //APP_CONFIG_H
