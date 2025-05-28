//
// Created by lozio on 20/05/25.
//

#ifndef IR_RECEIVER_MANAGER_H
#define IR_RECEIVER_MANAGER_H

#include <Arduino.h>

// Longueur attendue pour le code d'accès
const byte IR_EXPECTED_CODE_LENGTH = 4;

typedef struct {
    String entered_digits;
    bool is_complete;
    // unsigned long last_digit_timestamp; // Optionnel: pour un futur timeout entre les chiffres
} IRCodeBuffer;

// Initialisation du module IR (à appeler dans setup())
void ir_manager_setup();

// Fonction principale du gestionnaire IR (à appeler régulièrement, ex: dans une tâche de l'ordonnanceur)
// Retourne le caractère mappé si une nouvelle touche pertinente a été pressée, sinon '\0'
char ir_manager_handle_new_keypress();

// Vérifie si un code complet est disponible et le récupère dans 'output_code_buffer'
bool ir_manager_is_code_ready(String& output_code_buffer);

// Réinitialise l'état de la saisie du code
void ir_manager_reset_code_buffer();

//char ir_map_raw_code_to_char(uint64_t raw_ir_code);



#endif //IR_RECEIVER_MANAGER_H
