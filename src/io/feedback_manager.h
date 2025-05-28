#ifndef FEEDBACK_MANAGER_H
#define FEEDBACK_MANAGER_H

// NOUVEAU: Énumération pour les différents patterns de feedback
typedef enum {
    FB_PATTERN_NONE,
    FB_PATTERN_SUCCESS,
    FB_PATTERN_ERROR,
    FB_PATTERN_ALERT_MAX_ATTEMPTS,
    FB_PATTERN_ALERT_SUSPICIOUS_EXIT,
    FB_PATTERN_ALERT_FORCED_ENTRY
} FeedbackPatternType;

// Initialise le gestionnaire de retours utilisateur (LED, Buzzer)
void feedback_manager_setup();

// NOUVEAU: Demande l'exécution d'un pattern de feedback spécifique
// Cette fonction sera appelée par les anciennes fonctions feedback_signal_...
void feedback_request_pattern(FeedbackPatternType pattern);

// NOUVEAU: Tâche du gestionnaire de feedback, à appeler régulièrement par le scheduler
void feedback_manager_task();

// Les fonctions existantes seront conservées pour la compatibilité de l'interface,
// mais elles appelleront feedback_request_pattern()
void feedback_signal_success();
void feedback_signal_error();
void feedback_signal_alert_max_attempts();
void feedback_signal_alert_suspicious_exit();
void feedback_signal_alert_forced_entry();

#endif //FEEDBACK_MANAGER_H