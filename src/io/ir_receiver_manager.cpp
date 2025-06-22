//
// Created by lozio on 20/05/25.
//

#include "ir_receiver_manager.h"
#include "common/hardware_pins.h"

// Includi la libreria IRremoteESP8266
// Assicurati che la libreria sia installata correttamente tramite platformio.ini

#include <IRrecv.h>
// --- Definizione degli oggetti e variabili interne al modulo ---


// (Questi elementi non sono dichiarati nell'.h, quindi sono "privati" a questo file .cpp)

// Parametri per il ricevitore IR

const uint16_t INTERNAL_IR_CAPTURE_BUFFER_SIZE = 1024;
const uint8_t  INTERNAL_IR_TIMEOUT_MS = 15;

// Oggetto IRrecv (inizializzato in ir\_manager\_setup)
// Lo dichiariamo qui ma lo inizializziamo in setup per poter passare IR\_RECEIVER\_PIN

static IRrecv* local_ir_receiver = nullptr;// Puntatore, sarà inizializzato in setup


// Oggetto per memorizzare i risultati della decodifica
static decode_results internal_ir_decoded_results;

// Buffer per lo stato dell’inserimento del codice
static IRCodeBuffer code_input_buffer;


// Struttura per mappare i codici IR ai caratteri

struct IRKeyMapping {
    uint64_t ir_code_value; // Il codice grezzo ricevuto dal telecomando

    char key_character;   // Il carattere corrispondente (es.: '1', '2', '#', '\*')

};

const IRKeyMapping ir_key_mappings[] = {
    // caratteri
    {0xFF6897ULL, '0'},   // codici !
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

// Calcola automaticamente il numero di elementi nella tabella di mappatura
const int number_of_ir_key_mappings = sizeof(ir_key_mappings) / sizeof(ir_key_mappings[0]);

//Funzione di mappatura interna (non serve dichiararla nel .h se uso locale)

static char map_raw_code_to_char_internal(uint64_t raw_ir_code) {
    for (int i = 0; i < number_of_ir_key_mappings; i++) {
        if (ir_key_mappings[i].ir_code_value == raw_ir_code) {
            return ir_key_mappings[i].key_character;
        }
    }
    if (raw_ir_code == 0xFFFFFFFFFFFFFFFFULL) { 
      // Gestisci il codice di ripetizione
   // Per ora, possiamo segnalarlo con un carattere speciale o ignorarlo.
  // Potremmo anche restituire l'ultimo carattere non ripetuto se lo memorizzassimo.
  // Per un codice d’accesso, ignorare la ripetizione è spesso la soluzione migliore.

        return '\0'; // Ignora restituendo nulla di mappabile

    }
    return '\0';
}

// ---Implementazione delle Funzioni Pubbliche---
void ir_manager_setup() {
   // Inizializzazione dinamica dell'oggetto irrecv
// Questo assicura che IR\_RECEIVER\_PIN sia definito correttamente prima della creazione dell'oggetto
    if (local_ir_receiver == nullptr) {// Crea l’oggetto solo se non esiste già

        local_ir_receiver = new IRrecv(IR_RECEIVER_PIN, INTERNAL_IR_CAPTURE_BUFFER_SIZE, INTERNAL_IR_TIMEOUT_MS, true);
    }
    local_ir_receiver->enableIRIn(); // Avvia il ricevitore IR
    ir_manager_reset_code_buffer(); // Inizializzare il buffer
    Serial.println("IR Manager: Initialized and ready.");
}

char ir_manager_handle_new_keypress() {
    if (local_ir_receiver == nullptr) return '\0'; // sicurezza se il setup non è stato richiamato

    char mapped_char = '\0';

    if (local_ir_receiver->decode(&internal_ir_decoded_results)) {
        // Debug del codice grezzo (può essere commentato più tardi)
       // Serial.print("IR raw: 0x");
       // serialPrintUint64(internal\_ir\_decoded\_results.value, HEX);
      // Serial.println();


        mapped_char = map_raw_code_to_char_internal(internal_ir_decoded_results.value);

        if (mapped_char != '\0') {
            if (code_input_buffer.is_complete) {
               // Se un codice completo è già presente e in attesa di essere letto,
              // potremmo ignorare i nuovi tasti oppure gestire un tasto "clear"

                if (mapped_char == 'C') {
                    ir_manager_reset_code_buffer();
                    Serial.println("IR Buffer: Cleared by user.");
                      // mapped\_char è sempre 'C', il chiamante può reagire

                } else {
                   // Un codice è completo, non si aggiungono più cifre
                   // Si restituisce il carattere 'C' se è stato premuto, altrimenti niente di utile qui

                    return (mapped_char == 'C' ? 'C' : '\0');
                }
            } else { // il buffer non è ancora pieno
                if (isDigit(mapped_char)) {
                    code_input_buffer.entered_digits += mapped_char;
                    // code_input_buffer.last_digit_timestamp = millis(); // per futuri timeout
                    if (code_input_buffer.entered_digits.length() == IR_EXPECTED_CODE_LENGTH) {
                        code_input_buffer.is_complete = true;
                    }
                } else if (mapped_char == 'C') { // commando clear
                    ir_manager_reset_code_buffer();
                    Serial.println("IR Buffer: Cleared by user.");
                }
              // Altri tasti come 'O' (OK) potrebbero essere gestiti qui
              // per indicare "fine inserimento" anche se ci sono meno di 4 cifre.

            }
        }
        local_ir_receiver->resume(); // pronto per il prossimo codice
    }
    return mapped_char; //Restituisce il carattere mappato (o '\0' se niente o non pertinente)

}

bool ir_manager_is_code_ready(String& output_code_buffer) {
    if (code_input_buffer.is_complete) {
        output_code_buffer = code_input_buffer.entered_digits;
        return true;
    }
    output_code_buffer = ""; // Assicurarsi che la stringa di uscita sia vuota se non c’è codice pronto

    return false;
}

void ir_manager_reset_code_buffer() {
    code_input_buffer.entered_digits = "";
    code_input_buffer.is_complete = false;
   // code\_input\_buffer.last\_digit\_timestamp = millis();
// Serial.println("Buffer IR: Reset."); // Opzionale per debug

}
