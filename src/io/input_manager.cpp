//
// Created by lozio on 20/05/25.
//

#include "input_manager.h"
#include "common/hardware_pins.h" // Per EXIT_BUTTON_PIN
#include <Arduino.h>                 // Per pinMode, digitalRead, millis

// ---Variabili statiche interne al modulo ---
static byte stable_exit_button_state = HIGH;          // stato stabile attuale del bottone
static byte last_raw_exit_button_reading = HIGH;   // Ultima lettura grezza per rilevare i cambiamenti
static unsigned long last_exit_button_change_time = 0; // Per il timer di anti-rimbalzo


const unsigned long EXIT_BUTTON_DEBOUNCE_DELAY_MS = 50;

// --- Implementazione delle Funzioni Pubbliche ---
void input_manager_setup() {
    pinMode(EXIT_BUTTON_PIN, INPUT); // con il pull-up esterno
    stable_exit_button_state = digitalRead(EXIT_BUTTON_PIN);
    last_raw_exit_button_reading = stable_exit_button_state;
    Serial.println("Input Manager: Exit button pin initialized.");
}

bool input_manager_is_exit_button_newly_pressed() {
    bool is_new_press = false;
    byte current_raw_reading = digitalRead(EXIT_BUTTON_PIN);

    // Se la lettura grezza attuale è diversa dalla precedente, si è verificato un cambiamento (o rimbalzo).
    if (current_raw_reading != last_raw_exit_button_reading) {
        last_exit_button_change_time = millis(); // Si registra l’ora di questo cambiamento.

    }

   // Se è trascorso abbastanza tempo dall’ultimo cambiamento (anti-rimbalzo)
    if ((millis() - last_exit_button_change_time) > EXIT_BUTTON_DEBOUNCE_DELAY_MS) {
      // Lo stato attuale (current_raw_reading) è ora considerato stabile.
      // Si verifica se questo stato stabile è diverso dal precedente stato stabile E se è una pressione.
        if (current_raw_reading != stable_exit_button_state) {
            stable_exit_button_state = current_raw_reading; //Aggiorna lo stato stabile conosciuto


            if (stable_exit_button_state == LOW) {
              // Il pulsante è appena passato allo stato premuto (LOW) in modo stabile
                is_new_press = true;
            }
        }
    }

    last_raw_exit_button_reading = current_raw_reading; // Memorizza la lettura per la prossima chiamata

    return is_new_press;
}
