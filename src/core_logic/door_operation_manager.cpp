// SmartLook/src/core_logic/door_operation_manager.cpp
// Version finale avec correction du message "Code Valide!"

#include "door_operation_manager.h"
#include "../io/door_motor_manager.h"
#include "../io/door_sensor_manager.h"
#include "../io/input_manager.h"
#include "../display/display_manager.h"
#include "../api/api_client.h"
#include "../wifi/wifi_manager.h"
#include "../io/feedback_manager.h"
#include "../common/app_config.h"

// --- Variabili Statiche Interne al Modulo ---

constexpr unsigned long AUTO_CLOSE_DELAY_MS = 15000;
constexpr unsigned long ERROR_DISPLAY_HOLD_MS = 3000;
constexpr unsigned long MOTOR_OPERATION_MAX_DURATION_MS = 20000;
constexpr unsigned long LOCKOUT_DURATION_MS = 60000;

static DoorOpState current_op_state = DoorOpState::IDLE_CLOSED;
static String code_to_validate = "";
static String last_valid_code_for_api_notify = "";
static unsigned long timer_activity_start_ms = 0;
static bool auto_close_is_scheduled = false;
static uint8_t failed_code_attempts = 0;
static bool previous_door_closed_sensor_state_for_forcing_detection;
static bool agent_is_legitimately_inside = false;

// --- Funzioni di supporto private (static) ---

static void reset_to_idle_state_based_on_sensors() {
    const bool is_closed = door_sensor_is_door_fully_closed();
    const bool is_open = door_sensor_is_door_fully_opened();

    if (is_closed && !is_open) {
        current_op_state = DoorOpState::IDLE_CLOSED;
    } else if (is_open && !is_closed) {
        current_op_state = DoorOpState::IDLE_OPEN;
        timer_activity_start_ms = millis();
        auto_close_is_scheduled = true;
    } else {
        current_op_state = DoorOpState::IDLE_AJAR;
    }
}

static void log_and_display_state_change(DoorOpState previous_state, const char* event_trigger = "") {
    if (current_op_state == previous_state && strlen(event_trigger) == 0 &&
        current_op_state != DoorOpState::MOTOR_OPENING &&
        current_op_state != DoorOpState::MOTOR_CLOSING &&
        current_op_state != DoorOpState::LOCKED_OUT ) {
        return;
    }
    Serial.print("DoorOpMgr: ");
    if (strlen(event_trigger) > 0) { Serial.print("["); Serial.print(event_trigger); Serial.print("] "); }
    if (current_op_state != previous_state) {
        Serial.print("State Change "); Serial.print(static_cast<int>(previous_state));
        Serial.print(" -> "); Serial.println(static_cast<int>(current_op_state));
    } else {
        Serial.print("Current State (log for event/moving/lockout) "); Serial.println(static_cast<int>(current_op_state));
    }
    String msg = ""; bool clear_lcd_line0 = false;
    switch (current_op_state) {
        case DoorOpState::IDLE_CLOSED: msg = "Porte Fermee"; auto_close_is_scheduled = false; break;
        case DoorOpState::IDLE_OPEN: msg = (auto_close_is_scheduled) ? "P.Ouverte(FAuto)" : "Porte Ouverte"; break;
        case DoorOpState::IDLE_AJAR: msg = "Porte Milieu"; auto_close_is_scheduled = false; break;
        case DoorOpState::PENDING_API_VALIDATION: msg = "Validation API.."; break;
        case DoorOpState::MOTOR_OPENING: msg = "Ouverture..."; break;
        case DoorOpState::MOTOR_CLOSING: msg = "Fermeture..."; break;
        case DoorOpState::MOTOR_TIMEOUT: msg = "Err: Moteur TO"; clear_lcd_line0 = true; break;
        case DoorOpState::API_ERROR: msg = "Erreur API/WiFi"; clear_lcd_line0 = true; break;
        case DoorOpState::LOCKED_OUT: msg = "SYSTEME BLOQUE"; clear_lcd_line0 = true; break;
        case DoorOpState::INVALID_CODE_INFO: msg = "Code Invalide"; clear_lcd_line0 = true; break;
        default: msg = "Etat Inconnu"; break;
    }
    if (clear_lcd_line0) {
        display_update_input_code("");
    }
    if (!msg.isEmpty()) {
        display_show_message(msg, (current_op_state == DoorOpState::LOCKED_OUT ? 0 : 1), true);
    }
}

// --- Funzioni Pubbliche ---

void door_op_manager_setup() {
    reset_to_idle_state_based_on_sensors();
    DoorOpState old_state_for_log = current_op_state;
    log_and_display_state_change(old_state_for_log, "SetupInitial");
    previous_door_closed_sensor_state_for_forcing_detection = door_sensor_is_door_fully_closed();
    agent_is_legitimately_inside = false;
}

void door_op_manager_task() {
    DoorOpState state_before_task_run = current_op_state;
    bool current_physical_door_closed_state = door_sensor_is_door_fully_closed();

    if (previous_door_closed_sensor_state_for_forcing_detection == true && current_physical_door_closed_state == false) {
        bool is_opening_legitimately = (current_op_state == DoorOpState::MOTOR_OPENING);
        if (!is_opening_legitimately) {
            Serial.println("DoorOpMgr_Task: !! ALERTE FORCAGE DE PORTE DETECTE !!");
            if (current_op_state == DoorOpState::LOCKED_OUT) {
                 Serial.println("  -> Occurred during LOCKED_OUT state!");
            }
            display_show_message("ALERTE FORCAGE!", 0, true);
            feedback_signal_alert_forced_entry();
            ApiResponse alert_resp = api_send_alert("door_forced_open", "Detection d'ouverture de porte non commandee.", "critical");
            if (alert_resp.network_success && alert_resp.action_success) {
                Serial.println("  -> Alerte de forçage envoyée.");
            } else {
                Serial.println("  -> Echec envoi alerte de forçage.");
            }
        }
    }
    previous_door_closed_sensor_state_for_forcing_detection = current_physical_door_closed_state;

    if (input_manager_is_exit_button_newly_pressed()) {
        Serial.println("DoorOpMgr_Task: Exit button pressed event.");
        DoorOpState state_before_exit_press_logic = current_op_state;
        if (current_op_state == DoorOpState::IDLE_CLOSED || current_op_state == DoorOpState::IDLE_AJAR ||
            current_op_state == DoorOpState::IDLE_OPEN || current_op_state == DoorOpState::API_ERROR ||
            current_op_state == DoorOpState::MOTOR_TIMEOUT || current_op_state == DoorOpState::LOCKED_OUT ||
            current_op_state == DoorOpState::INVALID_CODE_INFO) {
            Serial.println("  -> Exit: Processing request.");
            if(current_op_state == DoorOpState::LOCKED_OUT) {
                Serial.println("  -> Exit initiated while system was LOCKED_OUT.");
            }
            if (agent_is_legitimately_inside) {
                Serial.println("  -> Exit: Button pressed by a presumed legitimate agent.");
                display_show_message("Sortie Normale", 1, true);
                feedback_signal_success();
                ApiResponse exit_log_resp = api_notify_door_close("_LBE_");
                if(exit_log_resp.network_success && exit_log_resp.http_code == 201 && exit_log_resp.action_success) {
                    Serial.println("  -> 'door_close' (_LBE_) notification sent.");
                } else {
                    Serial.print("  -> Failed 'door_close' (_LBE_) notification. HTTP: "); Serial.print(exit_log_resp.http_code); Serial.print(", R: "); Serial.println(exit_log_resp.reason);
                }
                agent_is_legitimately_inside = false;
            } else {
                Serial.println("  -> Exit: Button pressed with NO active legitimate entry session. SUSPICIOUS EXIT.");
                display_show_message("SORTIE SUSPECTE", 1, true);
                feedback_signal_alert_suspicious_exit();
                ApiResponse alert_resp = api_send_alert("suspicious_exit_button", "Bouton sortie utilise sans session d'entree valide.", "high");
                if (alert_resp.network_success && alert_resp.action_success) {
                    Serial.println("  -> Suspicious exit alert sent.");
                } else {
                    Serial.println("  -> Failed to send suspicious exit alert.");
                }
                ApiResponse exit_log_resp = api_notify_door_close("");
                if(exit_log_resp.network_success && exit_log_resp.http_code == 201 && exit_log_resp.action_success) {
                    Serial.println("  -> 'door_close' (empty code for suspicious) notification sent.");
                } else {
                    Serial.print("  -> Failed 'door_close' (empty suspicious) notification. HTTP: "); Serial.print(exit_log_resp.http_code); Serial.print(", R: "); Serial.println(exit_log_resp.reason);
                }
            }
            if (door_sensor_is_door_fully_opened()) {
                current_op_state = DoorOpState::IDLE_OPEN;
                auto_close_is_scheduled = true;
                timer_activity_start_ms = millis();
            } else {
                door_motor_open();
                timer_activity_start_ms = millis();
                current_op_state = DoorOpState::MOTOR_OPENING;
                auto_close_is_scheduled = true;
            }
        } else {
            Serial.print("  -> Exit button ignored. Current state: "); Serial.println(static_cast<int>(current_op_state));
        }
        if (current_op_state != state_before_exit_press_logic) {
             log_and_display_state_change(state_before_exit_press_logic, "ExitButton");
        }
    }

    switch (current_op_state) {
        case DoorOpState::IDLE_CLOSED:
        case DoorOpState::IDLE_AJAR: {
            if (!code_to_validate.isEmpty()) {
                current_op_state = DoorOpState::PENDING_API_VALIDATION;
            }
            break;
        }

        case DoorOpState::IDLE_OPEN: {
            if (auto_close_is_scheduled && (millis() - timer_activity_start_ms >= AUTO_CLOSE_DELAY_MS)) {
                bool is_currently_closed_by_sensor = door_sensor_is_door_fully_closed();
                if (!is_currently_closed_by_sensor) {
                    door_motor_close();
                    timer_activity_start_ms = millis();
                    current_op_state = DoorOpState::MOTOR_CLOSING;
                } else {
                    auto_close_is_scheduled = false;
                    current_op_state = DoorOpState::IDLE_CLOSED;
                    if (!last_valid_code_for_api_notify.isEmpty()) {
                        ApiResponse response = api_notify_door_close(last_valid_code_for_api_notify);
                        if(response.network_success && response.http_code == 201 && response.action_success) {
                            Serial.println("  -> API 'door_close' (finalize code for already_closed door): OK.");
                        } else {
                            Serial.println("  -> API 'door_close' (finalize code for already_closed door): FAILED.");
                        }
                        last_valid_code_for_api_notify = "";
                    }
                }
            }
            break;
        }

        case DoorOpState::PENDING_API_VALIDATION: {
            if (!wifi_is_connected()) {
                feedback_signal_error();
                current_op_state = DoorOpState::API_ERROR;
                timer_activity_start_ms = millis();
                code_to_validate = "";
            } else {
                const ApiResponse api_response = api_attempt_door_open(code_to_validate);
                if (api_response.network_success && api_response.http_code == 201 && api_response.action_success) {
                    feedback_signal_success();
                    last_valid_code_for_api_notify = code_to_validate;
                    agent_is_legitimately_inside = true;
                    code_to_validate = "";
                    failed_code_attempts = 0;
                    if (!door_sensor_is_door_fully_opened()) {
                        door_motor_open();
                        timer_activity_start_ms = millis();
                        current_op_state = DoorOpState::MOTOR_OPENING;
                        auto_close_is_scheduled = true;
                    } else {
                        current_op_state = DoorOpState::IDLE_OPEN;
                        timer_activity_start_ms = millis();
                        auto_close_is_scheduled = true;
                    }
                } else {
                    Serial.print("DoorOpMgr_Task: API: Code INVALIDE or Error. Reason: "); Serial.println(api_response.reason);

                    if (!api_response.network_success || (api_response.http_code != 201 && api_response.http_code != 200 && api_response.http_code != 0)) {
                        feedback_signal_error();
                    } else if (api_response.reason.indexOf("code_incorrect") != -1) {
                        feedback_signal_error();
                        failed_code_attempts++;
                        Serial.print("DoorOpMgr_Task: Failed attempt count: "); Serial.println(failed_code_attempts);
                        if (failed_code_attempts >= app_settings.max_failed_attempts) {
                            Serial.println("DoorOpMgr_Task: Max failed attempts reached. Sending alert and LOCKING OUT.");
                            display_show_message("ALERTE MAX TENTA", 0, true);
                            feedback_signal_alert_max_attempts();
                            ApiResponse alert_api_resp = api_send_alert("multiple_failed_attempts", String(app_settings.max_failed_attempts) + " tentatives code echouees", "high");
                            if (alert_api_resp.network_success && alert_api_resp.action_success) {
                                Serial.println("DoorOpMgr_Task: Alert sent successfully.");
                            } else {
                                Serial.println("DoorOpMgr_Task: Failed to send alert.");
                            }
                            failed_code_attempts = 0;
                            current_op_state = DoorOpState::LOCKED_OUT;
                            timer_activity_start_ms = millis();
                            code_to_validate = "";
                        }
                    } else if (api_response.reason.indexOf("code_expired") != -1 || api_response.reason.indexOf("code_already_used") != -1 || api_response.reason.indexOf("no_code_generated") != -1) {
                        feedback_signal_error();
                    }

                    if (current_op_state != DoorOpState::LOCKED_OUT) {
                        code_to_validate = "";
                        current_op_state = DoorOpState::INVALID_CODE_INFO;
                        timer_activity_start_ms = millis();
                    }
                }
            }
            break;
        }

        case DoorOpState::MOTOR_OPENING: {
            bool is_open_sensor_now_active = door_sensor_is_door_fully_opened();
            if (is_open_sensor_now_active) {
                door_motor_stop();
                Serial.println("DoorOpMgr_Task: Door fully open (sensor).");
                current_op_state = DoorOpState::IDLE_OPEN;
                timer_activity_start_ms = millis();
                auto_close_is_scheduled = true;
                Serial.println("DoorOpMgr_Task: Auto-close timer started for IDLE_OPEN state.");
            } else if (millis() - timer_activity_start_ms > MOTOR_OPERATION_MAX_DURATION_MS) {
                Serial.println("DoorOpMgr_Task: MOTOR TIMEOUT during opening!");
                feedback_signal_error();
                door_motor_stop();
                current_op_state = DoorOpState::MOTOR_TIMEOUT;
                timer_activity_start_ms = millis();
            }
            break;
        }

        case DoorOpState::MOTOR_CLOSING: {
            bool is_closed_sensor_now_active = door_sensor_is_door_fully_closed();
            if (is_closed_sensor_now_active) {
                door_motor_stop();
                Serial.println("DoorOpMgr_Task: Door physically closed (sensor).");
                if (!last_valid_code_for_api_notify.isEmpty()) {
                    ApiResponse response = api_notify_door_close(last_valid_code_for_api_notify);
                    if(response.network_success && response.http_code == 201 && response.action_success) {
                        Serial.println("  -> API 'door_close' (finalize entry code) OK.");
                    } else {
                        Serial.println("  -> API 'door_close' (finalize entry code) FAILED.");
                    }
                    last_valid_code_for_api_notify = "";
                } else {
                    Serial.println("  -> Door closed. No entry code pending finalization.");
                }
                current_op_state = DoorOpState::IDLE_CLOSED;
                auto_close_is_scheduled = false;
            } else if (millis() - timer_activity_start_ms > MOTOR_OPERATION_MAX_DURATION_MS) {
                Serial.println("DoorOpMgr_Task: MOTOR TIMEOUT during closing!");
                feedback_signal_error();
                door_motor_stop();
                current_op_state = DoorOpState::MOTOR_TIMEOUT;
                timer_activity_start_ms = millis();
            }
            break;
        }

        case DoorOpState::INVALID_CODE_INFO:
        case DoorOpState::MOTOR_TIMEOUT:
        case DoorOpState::API_ERROR: {
            if (millis() - timer_activity_start_ms > ERROR_DISPLAY_HOLD_MS) {
                display_update_input_code("");
                display_clear_line(1);
                reset_to_idle_state_based_on_sensors();
            }
            break;
        }

        case DoorOpState::LOCKED_OUT: {
            unsigned long time_elapsed_in_lockout = millis() - timer_activity_start_ms;
            if (time_elapsed_in_lockout < LOCKOUT_DURATION_MS) {
                String time_remaining_msg = "Restant: " + String((LOCKOUT_DURATION_MS - time_elapsed_in_lockout) / 1000) + "s";
                display_show_message(time_remaining_msg, 1, true);
            } else {
                Serial.println("DoorOpMgr_Task: Lockout duration ended. Returning to IDLE state.");
                display_clear_line(0);
                display_clear_line(1);
                reset_to_idle_state_based_on_sensors();
            }
            break;
        }
    }

    if (current_op_state != state_before_task_run || current_op_state == DoorOpState::LOCKED_OUT ||
        current_op_state == DoorOpState::MOTOR_OPENING || current_op_state == DoorOpState::MOTOR_CLOSING) {
        log_and_display_state_change(state_before_task_run);
    }
}

void door_op_request_entry(const String& ir_code) {
    if (current_op_state == DoorOpState::LOCKED_OUT) {
        Serial.println("DoorOpMgr_ExternalReq: System is LOCKED_OUT. IR code ignored.");
        feedback_signal_error();
        return;
    }
    Serial.print("DoorOpMgr_ExternalReq: Code '"); Serial.print(ir_code); Serial.println("' pour entrée.");
    if (current_op_state == DoorOpState::IDLE_CLOSED || current_op_state == DoorOpState::IDLE_AJAR ||
        current_op_state == DoorOpState::API_ERROR || current_op_state == DoorOpState::MOTOR_TIMEOUT ||
        current_op_state == DoorOpState::INVALID_CODE_INFO) {
        code_to_validate = ir_code;
        Serial.println("  -> Code mis en attente pour validation API.");
    } else {
        Serial.print("  -> Requête entrée ignorée. État actuel: "); Serial.println(static_cast<int>(current_op_state));
    }
}