// SmartLook/src/io/feedback_manager.h


#ifndef FEEDBACK_MANAGER_H
#define FEEDBACK_MANAGER_H

// Enumerazione per i diversi pattern di feedback

typedef enum {
    FB_PATTERN_NONE,
    FB_PATTERN_SUCCESS,
    FB_PATTERN_ERROR,
    FB_PATTERN_ALERT_MAX_ATTEMPTS,
    FB_PATTERN_ALERT_SUSPICIOUS_EXIT,
    FB_PATTERN_ALERT_FORCED_ENTRY
} FeedbackPatternType;

// Inizializza il gestore dei feedback utente (LED, Buzzer)

void feedback_manager_setup();
// Task del gestore dei feedback, da chiamare regolarmente dallo scheduler

void feedback_manager_task();

// Funzioni pubbliche per richiedere un pattern di feedback

void feedback_signal_success();
void feedback_signal_error();
void feedback_signal_alert_max_attempts();
void feedback_signal_alert_suspicious_exit();
void feedback_signal_alert_forced_entry();
void feedback_stop(); //funzione per fermare esplicitamente il feedback feedback

#endif //FEEDBACK_MANAGER_H
