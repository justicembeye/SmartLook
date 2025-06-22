//
// Created by lozio on 20/05/25.
//

#include "door_motor_manager.h"
#include "../common/hardware_pins.h" // Per i pin motore
#include <Arduino.h>                 // Per il  pinMode, digitalWrite

// ---Implementazione delle Funzioni Pubbliche ---

void door_motor_manager_setup() {
    pinMode(MOTOR_L293D_IN1_PIN, OUTPUT);
    pinMode(MOTOR_L293D_IN2_PIN, OUTPUT);

 // Arrestare il motore per impostazione predefinita all'avvio

    digitalWrite(MOTOR_L293D_IN1_PIN, LOW);
    digitalWrite(MOTOR_L293D_IN2_PIN, LOW);

    Serial.println("Door Motor Manager: Pins initialized.");
}

// IMPORTANTE: Potrebbe essere necessario invertire la logica di HIGH/LOW per IN1/IN2
// qui sotto se il motore gira nel verso sbagliato per "aprire" o "chiudere".
// Dipende da come è montato il motore e da come sono collegati i suoi fili al L293D
.

void door_motor_open() {
    Serial.println("Motor: Opening...");
 // Esempio: IN1=HIGH, IN2=LOW per un verso
    digitalWrite(MOTOR_L293D_IN1_PIN, HIGH);
    digitalWrite(MOTOR_L293D_IN2_PIN, LOW);
}

void door_motor_close() {
    Serial.println("Motor: Closing...");
 // Esempio: IN1=LOW, IN2=HIGH per l'altro verso
    digitalWrite(MOTOR_L293D_IN1_PIN, LOW);
    digitalWrite(MOTOR_L293D_IN2_PIN, HIGH);
}

void door_motor_stop() {
    Serial.println("Motor: Stopping...");
    // IN1=LOW, IN2=LOW per frenare (alcuni L293D frenano anche con HIGH/HIGH)

    digitalWrite(MOTOR_L293D_IN1_PIN, LOW);
    digitalWrite(MOTOR_L293D_IN2_PIN, LOW);
}
