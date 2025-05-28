//
// Created by lozio on 20/05/25.
//

#include "input_manager.h"
#include "common/hardware_pins.h" // Pour EXIT_BUTTON_PIN
#include <Arduino.h>                 // Pour pinMode, digitalRead, millis

// --- Variables Statiques Internes au Module ---
static byte stable_exit_button_state = HIGH;          // État stable actuel du bouton
static byte last_raw_exit_button_reading = HIGH;    // Dernière lecture brute pour détecter les changements
static unsigned long last_exit_button_change_time = 0; // Pour le timer d'anti-rebond

const unsigned long EXIT_BUTTON_DEBOUNCE_DELAY_MS = 50;

// --- Implémentation des Fonctions Publiques ---
void input_manager_setup() {
    pinMode(EXIT_BUTTON_PIN, INPUT); // Avec pull-up externe
    stable_exit_button_state = digitalRead(EXIT_BUTTON_PIN);
    last_raw_exit_button_reading = stable_exit_button_state;
    Serial.println("Input Manager: Exit button pin initialized.");
}

bool input_manager_is_exit_button_newly_pressed() {
    bool is_new_press = false;
    byte current_raw_reading = digitalRead(EXIT_BUTTON_PIN);

    // Si la lecture brute actuelle est différente de la précédente, un changement (ou rebond) s'est produit.
    if (current_raw_reading != last_raw_exit_button_reading) {
        last_exit_button_change_time = millis(); // On note l'heure de ce changement.
    }

    // Si suffisamment de temps s'est écoulé depuis le dernier changement (anti-rebond)
    if ((millis() - last_exit_button_change_time) > EXIT_BUTTON_DEBOUNCE_DELAY_MS) {
        // L'état actuel (current_raw_reading) est maintenant considéré comme stable.
        // On vérifie si cet état stable est différent de l'ancien état stable ET si c'est un appui.
        if (current_raw_reading != stable_exit_button_state) {
            stable_exit_button_state = current_raw_reading; // Mettre à jour l'état stable connu

            if (stable_exit_button_state == LOW) {
                // Le bouton vient de passer à l'état pressé (LOW) de manière stable
                is_new_press = true;
            }
        }
    }

    last_raw_exit_button_reading = current_raw_reading; // Mémoriser la lecture brute pour le prochain appel
    return is_new_press;
}
