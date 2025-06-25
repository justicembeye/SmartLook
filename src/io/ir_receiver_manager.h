//
// Created by lozio on 20/05/25.
//

#ifndef IR_RECEIVER_MANAGER_H
#define IR_RECEIVER_MANAGER_H

#include <Arduino.h>

// lunghezza attesa per il primo codice di acesso
const byte IR_EXPECTED_CODE_LENGTH = 4;

typedef struct {
    String entered_digits;
    bool is_complete;


} IRCodeBuffer;

// Inizializzazione del modulo IR (da chiamare in setup())
void ir_manager_setup();

// Funzione principale del gestore IR (da chiamare regolarmente, es.: in un task dello scheduler)
// Restituisce il carattere mappato se è stato premuto un nuovo tasto pertinente, altrimenti '\0'
char ir_manager_handle_new_keypress();

// Verifica se un codice completo è disponibile e lo recupera in 'output_code_buffer'
bool ir_manager_is_code_ready(String& output_code_buffer);

// Reimposta lo stato dell’inserimento del codice
void ir_manager_reset_code_buffer();

// char ir_map_raw_code_to_char(uint64_t raw_ir_code);



#endif //IR_RECEIVER_MANAGER_H
