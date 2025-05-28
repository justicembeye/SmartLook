//
// Created by lozio on 20/05/25.
//

#include "door_motor_manager.h"
#include "../common/hardware_pins.h" // Pour les broches moteur
#include <Arduino.h>                 // Pour pinMode, digitalWrite

// --- Implémentation des Fonctions Publiques ---

void door_motor_manager_setup() {
    pinMode(MOTOR_L293D_IN1_PIN, OUTPUT);
    pinMode(MOTOR_L293D_IN2_PIN, OUTPUT);

    // Mettre le moteur à l'arrêt par défaut au démarrage
    digitalWrite(MOTOR_L293D_IN1_PIN, LOW);
    digitalWrite(MOTOR_L293D_IN2_PIN, LOW);

    Serial.println("Door Motor Manager: Pins initialized.");
}

// IMPORTANT: Vous devrez peut-être inverser la logique de HIGH/LOW pour IN1/IN2
// ci-dessous si votre moteur tourne dans le mauvais sens pour "ouvrir" ou "fermer".
// Cela dépend de comment votre moteur est monté et de comment ses fils sont connectés au L293D.

void door_motor_open() {
    Serial.println("Motor: Opening...");
    // Exemple: IN1=HIGH, IN2=LOW pour un sens
    digitalWrite(MOTOR_L293D_IN1_PIN, HIGH);
    digitalWrite(MOTOR_L293D_IN2_PIN, LOW);
}

void door_motor_close() {
    Serial.println("Motor: Closing...");
    // Exemple: IN1=LOW, IN2=HIGH pour l'autre sens
    digitalWrite(MOTOR_L293D_IN1_PIN, LOW);
    digitalWrite(MOTOR_L293D_IN2_PIN, HIGH);
}

void door_motor_stop() {
    Serial.println("Motor: Stopping...");
    // IN1=LOW, IN2=LOW pour freiner (certains L293D freinent aussi avec HIGH/HIGH)
    digitalWrite(MOTOR_L293D_IN1_PIN, LOW);
    digitalWrite(MOTOR_L293D_IN2_PIN, LOW);
}
