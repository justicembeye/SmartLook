//
// Created by lozio on 20/05/25.
//

#include "ir_receiver_manager.h"
#include "common/hardware_pins.h"

// Inclure la bibliothèque IRremoteESP8266
// Assurez-vous que la bibliothèque est bien installée via platformio.ini
#include <IRrecv.h>

// --- Définition des objets et variables internes au module ---
// (Ces éléments ne sont pas déclarés dans le .h, ils sont donc "privés" à ce fichier .cpp)

// Paramètres pour le récepteur IR
const uint16_t INTERNAL_IR_CAPTURE_BUFFER_SIZE = 1024;
const uint8_t  INTERNAL_IR_TIMEOUT_MS = 15;

// Objet IRrecv (initialisé dans ir_manager_setup)
// On le déclare ici mais on l'initialise dans setup pour pouvoir passer IR_RECEIVER_PIN
static IRrecv* local_ir_receiver = nullptr; // Pointeur, sera initialisé dans setup

// Objet pour stocker les résultats du décodage
static decode_results internal_ir_decoded_results;

// Buffer pour l'état de la saisie du code
static IRCodeBuffer code_input_buffer;


// Structure pour mapper les codes IR aux caractères
struct IRKeyMapping {
    uint64_t ir_code_value; // Le code brut reçu de la télécommande
    char key_character;   // Le caractère correspondant (ex : '1', '2', '#', '*')
};

const IRKeyMapping ir_key_mappings[] = {
    // Chiffres
    {0xFF6897ULL, '0'},   // Vos codes ici !
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

// Calculer automatiquement le nombre d'éléments dans la table de mappage
const int number_of_ir_key_mappings = sizeof(ir_key_mappings) / sizeof(ir_key_mappings[0]);

// Fonction de mappage interne (pas besoin de la déclarer dans le .h si usage local)
static char map_raw_code_to_char_internal(uint64_t raw_ir_code) {
    for (int i = 0; i < number_of_ir_key_mappings; i++) {
        if (ir_key_mappings[i].ir_code_value == raw_ir_code) {
            return ir_key_mappings[i].key_character;
        }
    }
    if (raw_ir_code == 0xFFFFFFFFFFFFFFFFULL) { // Gérer le code de répétition
        // Pour l'instant, on peut le signaler avec un caractère spécial ou l'ignorer.
        // On pourrait aussi retourner le dernier caractère non-répétition si on le stockait.
        // Pour un code d'accès, ignorer la répétition est souvent le mieux.
        return '\0'; // Ignorer en ne retournant rien de mappable
    }
    return '\0';
}

// --- Implémentation des Fonctions Publiques ---
void ir_manager_setup() {
    // Initialisation de l'objet irrecv dynamiquement
    // Cela permet de s'assurer que IR_RECEIVER_PIN est bien défini avant la création de l'objet
    if (local_ir_receiver == nullptr) { // Créez l'objet seulement s'il n'existe pas
        local_ir_receiver = new IRrecv(IR_RECEIVER_PIN, INTERNAL_IR_CAPTURE_BUFFER_SIZE, INTERNAL_IR_TIMEOUT_MS, true);
    }
    local_ir_receiver->enableIRIn(); // Démarre le récepteur IR
    ir_manager_reset_code_buffer(); // Initialise le buffer
    Serial.println("IR Manager: Initialized and ready.");
}

char ir_manager_handle_new_keypress() {
    if (local_ir_receiver == nullptr) return '\0'; // Sécurité si setup n'a pas été appelé

    char mapped_char = '\0';

    if (local_ir_receiver->decode(&internal_ir_decoded_results)) {
        // Débogage du code brut (peut être commenté plus tard)
        // Serial.print("IR raw: 0x");
        // serialPrintUint64(internal_ir_decoded_results.value, HEX);
        // Serial.println();

        mapped_char = map_raw_code_to_char_internal(internal_ir_decoded_results.value);

        if (mapped_char != '\0') {
            if (code_input_buffer.is_complete) {
                // Si un code complet est déjà là et en attente d'être lu,
                // on pourrait ignorer les nouvelles touches ou gérer une touche "clear"
                if (mapped_char == 'C') {
                    ir_manager_reset_code_buffer();
                    Serial.println("IR Buffer: Cleared by user.");
                    // mapped_char est toujours 'C', l'appelant peut réagir
                } else {
                    // Un code est complet, on n'ajoute plus de chiffres
                    // On retourne le caractère 'C' s'il a été pressé, sinon rien d'utile ici
                    return (mapped_char == 'C' ? 'C' : '\0');
                }
            } else { // Le buffer n'est pas encore plein
                if (isDigit(mapped_char)) {
                    code_input_buffer.entered_digits += mapped_char;
                    // code_input_buffer.last_digit_timestamp = millis(); // Pour futur timeout
                    if (code_input_buffer.entered_digits.length() == IR_EXPECTED_CODE_LENGTH) {
                        code_input_buffer.is_complete = true;
                    }
                } else if (mapped_char == 'C') { // Touche Clear
                    ir_manager_reset_code_buffer();
                    Serial.println("IR Buffer: Cleared by user.");
                }
                // D'autres touches comme 'O' (OK) pourraient être gérées ici
                // pour signifier "fin de saisie" même si moins de 4 chiffres.
            }
        }
        local_ir_receiver->resume(); // Prêt pour le prochain code
    }
    return mapped_char; // Retourne le caractère mappé (ou '\0' si rien ou non pertinent)
}

bool ir_manager_is_code_ready(String& output_code_buffer) {
    if (code_input_buffer.is_complete) {
        output_code_buffer = code_input_buffer.entered_digits;
        return true;
    }
    output_code_buffer = ""; // S'assurer que la chaîne de sortie est vide si pas de code prêt
    return false;
}

void ir_manager_reset_code_buffer() {
    code_input_buffer.entered_digits = "";
    code_input_buffer.is_complete = false;
    // code_input_buffer.last_digit_timestamp = millis();
    // Serial.println("IR Buffer: Reset."); // Optionnel pour débogage
}