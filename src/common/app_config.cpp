// src/common/app_config.cpp
#include "app_config.h"
#include "../api/api_client.h" // Pour appeler api_get_system_settings()
#include <Arduino.h>           // Pour Serial

// Definizione dell'istanza globale dei parametri dell'applicazione
ApplicationSettings app_settings;

void load_app_settings_from_api() {
    Serial.println("ConfigLoader: Tentative de chargement des paramètres système depuis l'API...");

    ApiSystemSettings retrieved_settings;
    bool success = false;
    const int MAX_LOAD_ATTEMPTS = 3;
    const unsigned long RETRY_DELAY_MS = 2000; // Ritardo tra i tentativi

    for (int attempt = 1; attempt <= MAX_LOAD_ATTEMPTS; ++attempt) {
        Serial.print("ConfigLoader: Tentative #"); Serial.println(attempt);
        retrieved_settings = api_get_system_settings();

        if (retrieved_settings.fetch_success) {
            app_settings.max_failed_attempts = retrieved_settings.max_attempts;
            app_settings.code_validity_seconds = retrieved_settings.code_validity_seconds;

            Serial.println("ConfigLoader: Paramètres système chargés avec succès depuis l'API :");
            Serial.print("  - Max Failed Attempts: "); Serial.println(app_settings.max_failed_attempts);
            Serial.print("  - Code Validity (seconds): "); Serial.println(app_settings.code_validity_seconds);
            success = true;
            break; // Uscire dal ciclo in caso di successo
        } else {
            Serial.println("ConfigLoader: Echec de la tentative de chargement depuis l'API.");
            if (attempt < MAX_LOAD_ATTEMPTS) {
                Serial.print("ConfigLoader: Nouvelle tentative dans "); Serial.print(RETRY_DELAY_MS / 1000); Serial.println("s...");
                delay(RETRY_DELAY_MS); // Attendere prima del tentativo successivo
            }
        }
    }

    if (!success) {
        Serial.println("ConfigLoader: Echec final du chargement des paramètres depuis l'API après plusieurs tentatives. Utilisation des valeurs par défaut.");
        // I valori predefiniti sono già definiti nel costruttore di ApplicationSettings,
        // ma possiamo mostrarli per conferma.
        Serial.print("  - Using Default Max Failed Attempts: "); Serial.println(app_settings.max_failed_attempts);
        Serial.print("  - Using Default Code Validity (seconds): "); Serial.println(app_settings.code_validity_seconds);
    }
}