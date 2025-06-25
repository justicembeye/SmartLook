//
// Created by lozio on 20/05/25.
//

#include "ir_receiver_manager.h"
#include "common/hardware_pins.h"

// Inclure la bibliothèque IRremoteESP8266
// Assurez-vous que la bibliothèque est bien installée via platformio.ini
#include <IRrecv.h>

// --- Definizione di oggetti e variabili interne al modulo ---
// (Questi elementi non sono dichiarati nel .h, quindi sono "privati" a questo file .cpp)

// Parametri per il ricevitore IR
const uint16_t INTERNAL_IR_CAPTURE_BUFFER_SIZE = 1024;
const uint8_t  INTERNAL_IR_TIMEOUT_MS = 15;

// Oggetto IRrecv (inizializzato in ir_manager_setup)
// Lo dichiariamo qui ma lo inizializziamo in setup per poter passare IR_RECEIVER_PIN
static IRrecv* local_ir_receiver = nullptr; // Puntatore, sarà inizializzato in setup

// Oggetto per memorizzare i risultati della decodifica
static decode_results internal_ir_decoded_results;

// Buffer per lo stato dell'immissione del codice
static IRCodeBuffer code_input_buffer;


// Struttura per mappare i codici IR ai caratteri
struct IRKeyMapping {
    uint64_t ir_code_value; // Il codice ricevuto dal telecomando
    char key_character;   // Il carattere corrispondente (es: '1', '2', '#', '*')
};

const IRKeyMapping ir_key_mappings[] = {
    // Cifre
    {0xFF6897ULL, '0'},
    {0xFF30CFULL, '1'},
    {0xFF18E7ULL, '2'},
    {0xFF7A85ULL, '3'},
    {0xFF10EFULL, '4'},
    {0xFF38C7ULL, '5'},
    {0xFF5AA5ULL, '6'},
    {0xFF42BDULL, '7'},
    {0xFF4AB5ULL, '8'},
    {0xFF52ADULL, '9'},
    // Actions
    {0xFF02FDULL, 'O'}, // OK
    {0xFFE21DULL, 'C'}  // Clear
};

// Calcolare automaticamente il numero di elementi nella tabella di mappatura
const int number_of_ir_key_mappings = sizeof(ir_key_mappings) / sizeof(ir_key_mappings[0]);

// Funzione di mappatura interna
static char map_raw_code_to_char_internal(uint64_t raw_ir_code) {
    for (int i = 0; i < number_of_ir_key_mappings; i++) {
        if (ir_key_mappings[i].ir_code_value == raw_ir_code) {
            return ir_key_mappings[i].key_character;
        }
    }
    if (raw_ir_code == 0xFFFFFFFFFFFFFFFFULL) { // Gestire il codice di ripetizione
        // Per ora, possiamo segnalarlo con un carattere speciale o ignorarlo.
        // Potremmo anche restituire l'ultimo carattere non ripetuto se lo memorizzassimo.
        // Per un codice di accesso, ignorare la ripetizione è spesso la scelta migliore.
        return '\0'; // Ignorare senza restituire nulla di mappabile
    }
    return '\0';
}

// --- Implementazione delle Funzioni Pubbliche ---
void ir_manager_setup() {
    // Inizializzazione dinamica dell'oggetto irrecv
    // Questo assicura che IR_RECEIVER_PIN sia definito correttamente prima della creazione dell'oggetto
    if (local_ir_receiver == nullptr) { // Crea l'oggetto solo se non esiste
        local_ir_receiver = new IRrecv(IR_RECEIVER_PIN, INTERNAL_IR_CAPTURE_BUFFER_SIZE, INTERNAL_IR_TIMEOUT_MS, true);
    }
    local_ir_receiver->enableIRIn(); // Avvia il ricevitore IR
    ir_manager_reset_code_buffer(); // Inizializza il buffer
    Serial.println("IR Manager: Initialized and ready.");
}

char ir_manager_handle_new_keypress() {
    if (local_ir_receiver == nullptr) return '\0';

    char mapped_char = '\0';

    if (local_ir_receiver->decode(&internal_ir_decoded_results)) {

        mapped_char = map_raw_code_to_char_internal(internal_ir_decoded_results.value);

        if (mapped_char != '\0') {
            if (code_input_buffer.is_complete) {
                // Se un codice completo è già presente e in attesa di essere letto,
                // potremmo ignorare i nuovi tasti o gestire un tasto "clear"
                if (mapped_char == 'C') {
                    ir_manager_reset_code_buffer();
                    Serial.println("IR Buffer: Cleared by user.");
                } else {
                    return (mapped_char == 'C' ? 'C' : '\0');
                }
            } else { // Il buffer non è ancora pieno
                if (isDigit(mapped_char)) {
                    code_input_buffer.entered_digits += mapped_char;
                    if (code_input_buffer.entered_digits.length() == IR_EXPECTED_CODE_LENGTH) {
                        code_input_buffer.is_complete = true;
                    }
                } else if (mapped_char == 'C') { // Tasto Clear
                    ir_manager_reset_code_buffer();
                    Serial.println("IR Buffer: Cleared by user.");
                }

            }
        }
        local_ir_receiver->resume();
    }
    return mapped_char; // Restituisce il carattere mappato (o '\0' se nulla o non pertinente)
}

bool ir_manager_is_code_ready(String& output_code_buffer) {
    if (code_input_buffer.is_complete) {
        output_code_buffer = code_input_buffer.entered_digits;
        return true;
    }
    output_code_buffer = ""; // Assicurarsi che la stringa di uscita sia vuota se non c'è codice pronto
    return false;
}

void ir_manager_reset_code_buffer() {
    code_input_buffer.entered_digits = "";
    code_input_buffer.is_complete = false;
}