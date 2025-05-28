#include "feedback_manager.h"
#include "../common/hardware_pins.h" //
#include <Arduino.h>

// Durées pour les signaux (en millisecondes) - existantes
const unsigned long SUCCESS_SIGNAL_DURATION_MS = 500;
const unsigned long SUCCESS_BEEP_DURATION_MS = 100;
const unsigned long ERROR_SIGNAL_DURATION_MS = 700;

// Paramètres pour alerte MAX_ATTEMPTS - existants
const unsigned long MAX_ATTEMPTS_BLINK_ON_MS = 150; //
const unsigned long MAX_ATTEMPTS_BLINK_OFF_MS = 100; //
const int MAX_ATTEMPTS_BLINK_COUNT = 5; //

// Paramètres pour alerte SUSPICIOUS_EXIT - existants
const unsigned long SUSPICIOUS_EXIT_DURATION_MS = 1500;
const int SUSPICIOUS_EXIT_BEEP_TONE = 1500;
const int SUSPICIOUS_EXIT_BEEP_DURATION = 150;

// Paramètres pour alerte FORCED_ENTRY - existants
const unsigned long FORCED_ENTRY_BLINK_ALTERNATE_MS = 100;
const int FORCED_ENTRY_CYCLE_COUNT = 10; // Total (FORCED_ENTRY_CYCLE_COUNT * 2) phases
const int FORCED_ENTRY_BUZZER_TONE = 2500;


// --- NOUVELLES VARIABLES GLOBALES STATIQUES POUR LA MACHINE D'ETAT DU FEEDBACK ---
static FeedbackPatternType current_pattern = FB_PATTERN_NONE;
static unsigned long pattern_timer_start_ms = 0;
static unsigned long last_toggle_ms = 0;
static int current_blink_count = 0;
static bool led_is_on = false; // Pour les clignotements simples
static byte rgb_state_machine_phase = 0; // Pour les patterns plus complexes comme l'alternance

// Fonction privée pour définir la couleur de la LED RGB (Anode Commune) - existante
static void set_rgb_color(byte red_on, byte green_on, byte blue_on) {
    digitalWrite(RGB_LED_RED_PIN,   red_on   ? LOW : HIGH); //
    digitalWrite(RGB_LED_GREEN_PIN, green_on ? LOW : HIGH); //
    digitalWrite(RGB_LED_BLUE_PIN,  blue_on  ? LOW : HIGH); //
}

void feedback_manager_setup() {
    pinMode(RGB_LED_RED_PIN, OUTPUT);
    pinMode(RGB_LED_GREEN_PIN, OUTPUT);
    pinMode(RGB_LED_BLUE_PIN, OUTPUT);
    pinMode(BUZZER_PIN, OUTPUT); //
    set_rgb_color(0, 0, 0);
    digitalWrite(BUZZER_PIN, LOW);
    Serial.println("Feedback Manager: Initialized (non-blocking).");
}

// NOUVEAU: Fonction pour demander un pattern
void feedback_request_pattern(FeedbackPatternType pattern) {
    // Si un pattern est déjà en cours, on pourrait décider de le prioriser ou de l'ignorer.
    // Pour l'instant, le nouveau pattern écrase l'ancien, sauf si c'est NONE.
    // Une logique plus fine pourrait être ajoutée ici si nécessaire (ex: priorités d'alertes).
    // if (pattern == FB_PATTERN_NONE && current_pattern == FB_PATTERN_NONE) return;


    Serial.print("FeedbackMgr: Requesting pattern: "); Serial.println(pattern);
    current_pattern = pattern;
    pattern_timer_start_ms = millis();
    last_toggle_ms = millis(); // Important pour le premier cycle
    current_blink_count = 0;
    led_is_on = false; // S'assurer que le premier cycle de clignotement commence par allumer
    rgb_state_machine_phase = 0;

    // Actions initiales spécifiques au pattern (ex: allumer un son continu pour forçage)
    if (pattern == FB_PATTERN_ALERT_FORCED_ENTRY) {
        tone(BUZZER_PIN, FORCED_ENTRY_BUZZER_TONE); // Son aigu et continu pour forçage
    } else {
        noTone(BUZZER_PIN); // S'assurer que les sons des patterns précédents sont coupés
    }
     // S'assurer que les LEDs sont éteintes avant de commencer un nouveau pattern (sauf si le pattern commence allumé)
    if (pattern != FB_PATTERN_NONE) { // Pour éviter d'éteindre si on demande explicitement NONE alors que c'est déjà NONE
       // set_rgb_color(0,0,0); // Fait par la fin des patterns
    }
}

// Fonctions publiques existantes, maintenant elles appellent feedback_request_pattern
void feedback_signal_success() {
    Serial.println("FeedbackMgr: Signaling Success (request)");
    feedback_request_pattern(FB_PATTERN_SUCCESS);
}

void feedback_signal_error() {
    Serial.println("FeedbackMgr: Signaling Simple Error (request)");
    feedback_request_pattern(FB_PATTERN_ERROR);
}

void feedback_signal_alert_max_attempts() {
    Serial.println("FeedbackMgr: Signaling Alert MAX_ATTEMPTS (request)");
    feedback_request_pattern(FB_PATTERN_ALERT_MAX_ATTEMPTS);
}

void feedback_signal_alert_suspicious_exit() {
    Serial.println("FeedbackMgr: Signaling Alert SUSPICIOUS_EXIT (request)");
    feedback_request_pattern(FB_PATTERN_ALERT_SUSPICIOUS_EXIT);
}

void feedback_signal_alert_forced_entry() {
    Serial.println("FeedbackMgr: Signaling Alert FORCED_ENTRY (request)");
    feedback_request_pattern(FB_PATTERN_ALERT_FORCED_ENTRY);
}


// NOUVEAU: Tâche principale du gestionnaire de feedback
void feedback_manager_task() {
    if (current_pattern == FB_PATTERN_NONE) {
        return;
    }

    unsigned long current_time_ms = millis();

    switch (current_pattern) {
        case FB_PATTERN_SUCCESS:
            if (current_blink_count == 0) { // Phase 1: Allumer LED et buzzer
                set_rgb_color(0, 1, 0); // Vert
                digitalWrite(BUZZER_PIN, HIGH);
                current_blink_count = 1;
                last_toggle_ms = current_time_ms; // Marquer le début du bip
            } else if (current_blink_count == 1) { // Phase 2: Attendre la fin du bip
                if (current_time_ms - last_toggle_ms >= SUCCESS_BEEP_DURATION_MS) {
                    digitalWrite(BUZZER_PIN, LOW);
                    current_blink_count = 2;
                    // last_toggle_ms n'est pas remis à jour ici, on se base sur pattern_timer_start_ms pour la durée totale
                }
            } else if (current_blink_count == 2) { // Phase 3: Attendre la fin de la signalisation globale
                if (current_time_ms - pattern_timer_start_ms >= SUCCESS_SIGNAL_DURATION_MS) {
                    set_rgb_color(0, 0, 0); // Éteindre LED
                    feedback_request_pattern(FB_PATTERN_NONE); // Fin du pattern
                }
            }
            break;

        case FB_PATTERN_ERROR:
            if (current_blink_count == 0) { // Phase 1: Allumer LED et buzzer
                set_rgb_color(1, 0, 0); // Rouge
                digitalWrite(BUZZER_PIN, HIGH);
                current_blink_count = 1;
            } else { // Phase 2: Attendre la fin de la signalisation
                if (current_time_ms - pattern_timer_start_ms >= ERROR_SIGNAL_DURATION_MS) {
                    set_rgb_color(0, 0, 0);
                    digitalWrite(BUZZER_PIN, LOW);
                    feedback_request_pattern(FB_PATTERN_NONE); // Fin du pattern
                }
            }
            break;

        case FB_PATTERN_ALERT_MAX_ATTEMPTS:
            // Objectif: MAX_ATTEMPTS_BLINK_COUNT clignotements (ON-OFF cycles)
            if (current_blink_count < MAX_ATTEMPTS_BLINK_COUNT * 2) { // Chaque clignotement a une phase ON et une phase OFF
                if (!led_is_on) { // Si LED est éteinte, on l'allume (après délai OFF)
                    if (current_time_ms - last_toggle_ms >= MAX_ATTEMPTS_BLINK_OFF_MS) {
                        set_rgb_color(1, 0, 0); // Rouge ON
                        tone(BUZZER_PIN, 2000, MAX_ATTEMPTS_BLINK_ON_MS / 2); // Bip court
                        led_is_on = true;
                        last_toggle_ms = current_time_ms;
                        current_blink_count++; // Compte une demi-phase (passage à ON)
                    }
                } else { // Si LED est allumée, on l'éteint (après délai ON)
                    if (current_time_ms - last_toggle_ms >= MAX_ATTEMPTS_BLINK_ON_MS) {
                        set_rgb_color(0, 0, 0); // LED OFF
                        // Le tone s'arrête de lui-même après sa durée.
                        led_is_on = false;
                        last_toggle_ms = current_time_ms;
                        current_blink_count++; // Compte une demi-phase (passage à OFF)
                    }
                }
            } else { // Tous les clignotements sont terminés
                noTone(BUZZER_PIN); // S'assurer que le buzzer est bien éteint
                set_rgb_color(0,0,0); // S'assurer que la LED est éteinte
                feedback_request_pattern(FB_PATTERN_NONE);
            }
            break;

        case FB_PATTERN_ALERT_SUSPICIOUS_EXIT:
            // Objectif: Couleur Jaune/Orange fixe, 3 bips distincts, puis maintenir couleur jusqu'à SUSPICIOUS_EXIT_DURATION_MS
            if (rgb_state_machine_phase == 0) { // Phase initiale: Allumer couleur, démarrer premier bip
                set_rgb_color(1, 1, 0); // Jaune/Orange (Rouge + Vert)
                tone(BUZZER_PIN, SUSPICIOUS_EXIT_BEEP_TONE, SUSPICIOUS_EXIT_BEEP_DURATION);
                last_toggle_ms = current_time_ms;
                rgb_state_machine_phase = 1; // En attente de la fin du premier bip + pause
                current_blink_count = 1; // Compteur de bips émis
            } else if (rgb_state_machine_phase == 1) { // En attente fin bip + pause
                if (current_time_ms - last_toggle_ms >= (unsigned long)(SUSPICIOUS_EXIT_BEEP_DURATION + 100)) {
                    if (current_blink_count < 3) { // S'il reste des bips à faire
                        tone(BUZZER_PIN, SUSPICIOUS_EXIT_BEEP_TONE, SUSPICIOUS_EXIT_BEEP_DURATION);
                        last_toggle_ms = current_time_ms;
                        current_blink_count++;
                        // Reste en phase 1 pour le prochain bip
                    } else { // Tous les bips sont faits
                        rgb_state_machine_phase = 2; // Passer à la phase de maintien de la couleur
                        // last_toggle_ms n'est pas changé, on utilise pattern_timer_start_ms pour la durée totale
                    }
                }
            } else if (rgb_state_machine_phase == 2) { // Maintien de la couleur
                if (current_time_ms - pattern_timer_start_ms >= SUSPICIOUS_EXIT_DURATION_MS) {
                    set_rgb_color(0, 0, 0);
                    noTone(BUZZER_PIN);
                    feedback_request_pattern(FB_PATTERN_NONE);
                }
            }
            break;

        case FB_PATTERN_ALERT_FORCED_ENTRY:
            // Objectif: Alternance Rouge/Bleu, son continu. Le son est démarré dans feedback_request_pattern.
            // FORCED_ENTRY_CYCLE_COUNT est le nombre de cycles ROUGE-BLEU. Donc *2 pour les phases.
            if (current_blink_count < FORCED_ENTRY_CYCLE_COUNT * 2) {
                if (current_time_ms - last_toggle_ms >= FORCED_ENTRY_BLINK_ALTERNATE_MS) {
                    if (rgb_state_machine_phase % 2 == 0) set_rgb_color(1, 0, 0); // Rouge
                    else set_rgb_color(0, 0, 1); // Bleu
                    rgb_state_machine_phase++;
                    last_toggle_ms = current_time_ms;
                    current_blink_count++;
                }
            } else {
                set_rgb_color(0, 0, 0);
                noTone(BUZZER_PIN); // Arrêter le son continu
                feedback_request_pattern(FB_PATTERN_NONE);
            }
            break;

        default:
            feedback_request_pattern(FB_PATTERN_NONE); // Pattern inconnu, on arrête tout.
            break;
    }
}