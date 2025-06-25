// SmartLook/src/display/display_manager.h
// (Fichier inchangé)

#ifndef DISPLAY_MANAGER_H
#define DISPLAY_MANAGER_H
#include <Arduino.h>

// inizializza lo schermo lcd ( a richiamare nel nostro principale)
void display_setup();

//stampa il codice in corso di digitazione . ad esempio : "code : 123:"
void display_update_input_code(const String& current_code_digits);

// stampa un messaggio dello stato su una  riga specifiche.
// riga 0 per la prima riga , riga 1 per la seconda
void display_show_message(const String& message, int line, bool clear_previous_line_content = true);

// cancella tutto lo schermo
void display_clear_all();

// cancella una riga scpifica dello schermo LCD
void display_clear_line(int line);

// funzione per scivere un carattere personalizzato a la posizione attuale del cursore
void display_write_custom_char(uint8_t char_index);

// Funzione per aggiornare l'icona dello stato WiFi
// funzione per aggiornare lo stato del wifi
void display_update_wifi_status_icon();

#endif //DISPLAY_MANAGER_H