// SmartLook/src/display/display_manager.h
// (Fichier inchangé)

#ifndef DISPLAY_MANAGER_H
#define DISPLAY_MANAGER_H
#include <Arduino.h>

// Initialise l'écran LCD (à appeler dans le setup principal)
void display_setup();

// Affiche le code en cours de saisie sur la première ligne
// Par exemple: "Code: 123"
void display_update_input_code(const String& current_code_digits);

// Affiche un message de statut sur une ligne spécifiée
// Ligne 0 pour la première ligne, Ligne 1 pour la seconde
void display_show_message(const String& message, int line, bool clear_previous_line_content = true);

// Efface complètement l'écran LCD
void display_clear_all();

// Efface une ligne spécifique de l'écran LCD
void display_clear_line(int line);

// NOUVEAU: Fonction pour écrire un caractère personnalisé à la position actuelle du curseur
void display_write_custom_char(uint8_t char_index);

// NOUVEAU: Fonction pour mettre à jour l'icône d'état WiFi
void display_update_wifi_status_icon();

#endif //DISPLAY_MANAGER_H