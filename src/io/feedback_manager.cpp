// SmartLook/src/io/feedback_manager.cpp


#include "feedback_manager.h"
#include "../common/hardware_pins.h"
#include <Arduino.h>
#include <cmath> 

// --- Parametri per la segnaletica di base ---
constexpr unsigned long SUCCESS_SIGNAL_DURATION_MS = 500;
constexpr unsigned long SUCCESS_BEEP_DURATION_MS = 100;
constexpr unsigned long ERROR_SIGNAL_DURATION_MS = 700;

// --- Parametri per la segnaletica migliorata ---
constexpr unsigned long MAX_ATTEMPTS_PULSE_PERIOD_MS = 2000;
constexpr int MAX_ATTEMPTS_PULSE_CYCLES = 3;
constexpr int MAX_ATTEMPTS_BEEP_TONE = 800;
constexpr unsigned long SUSPICIOUS_EXIT_BLINK_INTERVAL_MS = 150;
constexpr unsigned long SUSPICIOUS_EXIT_DURATION_MS = 3000;
constexpr int SUSPICIOUS_EXIT_BEEP_TONE = 2000;
constexpr unsigned long FORCED_ENTRY_BLINK_ALTERNATE_MS = 100;
constexpr unsigned long FORCED_ENTRY_DURATION_MS = 10000;
constexpr int FORCED_ENTRY_SIREN_TONE_1 = 2500;
constexpr int FORCED_ENTRY_SIREN_TONE_2 = 3200;
constexpr unsigned long FORCED_ENTRY_SIREN_INTERVAL_MS = 150;

// --- Variabili globali statiche per la macchina a stati ---
static FeedbackPatternType current_pattern = FB_PATTERN_NONE;
static unsigned long pattern_timer_start_ms = 0;
static unsigned long last_toggle_ms = 0;
static int current_cycle_count = 0;
static bool led_is_on = false;
static byte state_machine_phase = 0;


static void set_rgb_color_pwm(int r, int g, int b) {
    r = constrain(r, 0, 255);
    g = constrain(g, 0, 255);
    b = constrain(b, 0, 255);
    analogWrite(RGB_LED_RED_PIN, 255 - r);
    analogWrite(RGB_LED_GREEN_PIN, 255 - g);
    analogWrite(RGB_LED_BLUE_PIN, 255 - b);
}

static void stop_all_feedback() {
    set_rgb_color_pwm(0, 0, 0);
    noTone(BUZZER_PIN);
    current_pattern = FB_PATTERN_NONE;
}

// Funzione interna per avviare un nuovo pattern
static void start_pattern(FeedbackPatternType pattern) {
    Serial.print("FeedbackMgr: Requesting pattern: "); Serial.println(static_cast<int>(pattern));
    stop_all_feedback();
    current_pattern = pattern;
    pattern_timer_start_ms = millis();
    last_toggle_ms = millis();
    current_cycle_count = 0;
    led_is_on = false;
    state_machine_phase = 0;
}

// --- Funzione pubblica ---
void feedback_manager_setup() {
    pinMode(RGB_LED_RED_PIN, OUTPUT);
    pinMode(RGB_LED_GREEN_PIN, OUTPUT);
    pinMode(RGB_LED_BLUE_PIN, OUTPUT);
    pinMode(BUZZER_PIN, OUTPUT);
    stop_all_feedback();
    Serial.println("Feedback Manager: Initialized (PWM enabled).");
}

void feedback_stop() { stop_all_feedback(); }
void feedback_signal_success() { start_pattern(FB_PATTERN_SUCCESS); }
void feedback_signal_error() { start_pattern(FB_PATTERN_ERROR); }
void feedback_signal_alert_max_attempts() { start_pattern(FB_PATTERN_ALERT_MAX_ATTEMPTS); }
void feedback_signal_alert_suspicious_exit() { start_pattern(FB_PATTERN_ALERT_SUSPICIOUS_EXIT); }
void feedback_signal_alert_forced_entry() { start_pattern(FB_PATTERN_ALERT_FORCED_ENTRY); }

void feedback_manager_task() {
    if (current_pattern == FB_PATTERN_NONE) {
        return;
    }

    const unsigned long current_time_ms = millis();
    const unsigned long elapsed_time = current_time_ms - pattern_timer_start_ms;

    switch (current_pattern) {
        case FB_PATTERN_SUCCESS: {
            if (state_machine_phase == 0) {
                set_rgb_color_pwm(0, 255, 0);
                tone(BUZZER_PIN, 1500, SUCCESS_BEEP_DURATION_MS);
                state_machine_phase = 1;
            }
            if (elapsed_time >= SUCCESS_SIGNAL_DURATION_MS) {
                stop_all_feedback();
            }
            break;
        }

        case FB_PATTERN_ERROR: {
            if (state_machine_phase == 0) {
                set_rgb_color_pwm(255, 0, 0);
                tone(BUZZER_PIN, 500, ERROR_SIGNAL_DURATION_MS);
                state_machine_phase = 1;
            }
            if (elapsed_time >= ERROR_SIGNAL_DURATION_MS) {
                stop_all_feedback();
            }
            break;
        }

        case FB_PATTERN_ALERT_MAX_ATTEMPTS: {
            if (current_cycle_count >= MAX_ATTEMPTS_PULSE_CYCLES) {
                stop_all_feedback();
                break;
            }

           const double sine_wave = std::sin(elapsed_time * (TWO_PI / static_cast<double>(MAX_ATTEMPTS_PULSE_PERIOD_MS)));
            const int brightness = static_cast<int>(127.5 * (sine_wave + 1.0));
            set_rgb_color_pwm(brightness, brightness / 2, 0);

            if (state_machine_phase == 0 && sine_wave > 0.95) {
                tone(BUZZER_PIN, MAX_ATTEMPTS_BEEP_TONE, 80);
                state_machine_phase = 1;
            } else if (state_machine_phase == 1 && sine_wave < -0.95) {
                state_machine_phase = 2;
            } else if (state_machine_phase == 2 && sine_wave > 0.95) {
                tone(BUZZER_PIN, MAX_ATTEMPTS_BEEP_TONE, 80);
                state_machine_phase = 3;
                current_cycle_count++;
            } else if (state_machine_phase == 3 && sine_wave < -0.95) {
                 state_machine_phase = 0;
            }

            break;
        }

        case FB_PATTERN_ALERT_SUSPICIOUS_EXIT: {
            if (elapsed_time >= SUSPICIOUS_EXIT_DURATION_MS) {
                stop_all_feedback();
                break;
            }

            if (current_time_ms - last_toggle_ms >= SUSPICIOUS_EXIT_BLINK_INTERVAL_MS) {
                led_is_on = !led_is_on;
                if (led_is_on) {
                    set_rgb_color_pwm(255, 200, 0);
                    tone(BUZZER_PIN, SUSPICIOUS_EXIT_BEEP_TONE, 50);
                } else {
                    set_rgb_color_pwm(0, 0, 0);
                }
                last_toggle_ms = current_time_ms;
            }
            break;
        }

        case FB_PATTERN_ALERT_FORCED_ENTRY: {
            if (elapsed_time >= FORCED_ENTRY_DURATION_MS) {
                stop_all_feedback();
                break;
            }

            if (current_time_ms - last_toggle_ms >= FORCED_ENTRY_BLINK_ALTERNATE_MS) {
                led_is_on = !led_is_on;
                set_rgb_color_pwm(led_is_on ? 255 : 0, 0, led_is_on ? 0 : 255);
                last_toggle_ms = current_time_ms;
            }

            if (state_machine_phase == 0) {
                 tone(BUZZER_PIN, FORCED_ENTRY_SIREN_TONE_1);
                 state_machine_phase = 1;
            } else if (elapsed_time % (FORCED_ENTRY_SIREN_INTERVAL_MS * 2) < FORCED_ENTRY_SIREN_INTERVAL_MS) {
                if(state_machine_phase != 1) {
                    tone(BUZZER_PIN, FORCED_ENTRY_SIREN_TONE_1);
                    state_machine_phase = 1;
                }
            } else {
                 if(state_machine_phase != 2) {
                    tone(BUZZER_PIN, FORCED_ENTRY_SIREN_TONE_2);
                    state_machine_phase = 2;
                }
            }
            break;
        }

        default: {
            stop_all_feedback();
            break;
        }
    }
}
