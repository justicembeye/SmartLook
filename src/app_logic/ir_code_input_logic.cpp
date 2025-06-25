// src/app_logic/ir_code_input_logic.cpp
#include "ir_code_input_logic.h"
#include "../io/ir_receiver_manager.h"   // Pour IR_EXPECTED_CODE_LENGTH etc.
#include "../display/display_manager.h" // Pour display_update_input_code
#include "../core_logic/door_operation_manager.h" // Pour door_op_request_entry
#include <Arduino.h>                    // Pour Serial, millis, isDigit, String

// Namespace anonimo per variabili e tipi locali a questo file
namespace {
    enum class LocalIRInputState {
        WAITING_FOR_DIGIT,
        SHOWING_FULL_CODE_TEMP
    };
    LocalIRInputState current_ir_input_state = LocalIRInputState::WAITING_FOR_DIGIT;
    String current_display_code_ir = "";
    unsigned long full_code_display_start_time_ir = 0;
    const unsigned long IR_FULL_CODE_VISIBLE_DURATION_MS = 300;
}

void ir_code_input_logic_task() {
    char new_key_pressed = ir_manager_handle_new_keypress();

    switch (current_ir_input_state) {
        case LocalIRInputState::WAITING_FOR_DIGIT:
            if (new_key_pressed != '\0') {
                Serial.print("IRInputLogic: Key '"); Serial.print(new_key_pressed); Serial.println("'");
                if (isDigit(new_key_pressed)) {
                    if (current_display_code_ir.length() < IR_EXPECTED_CODE_LENGTH) {
                        current_display_code_ir += new_key_pressed;
                        display_update_input_code(current_display_code_ir);
                    }
                    if (current_display_code_ir.length() == IR_EXPECTED_CODE_LENGTH) {
                        current_ir_input_state = LocalIRInputState::SHOWING_FULL_CODE_TEMP;
                        full_code_display_start_time_ir = millis();
                        Serial.println("IRInputLogic: Full code entered, temp display.");
                    }
                } else if (new_key_pressed == 'C') {
                    current_display_code_ir = "";
                    ir_manager_reset_code_buffer();
                    display_update_input_code(current_display_code_ir);
                    Serial.println("IRInputLogic: Cleared.");
                }
            }
            break;

        case LocalIRInputState::SHOWING_FULL_CODE_TEMP:
            if (millis() - full_code_display_start_time_ir >= IR_FULL_CODE_VISIBLE_DURATION_MS) {
                String complete_code_from_buffer;
                if (ir_manager_is_code_ready(complete_code_from_buffer)) {
                    Serial.print("IRInputLogic: Temp display done. Code '"); Serial.print(complete_code_from_buffer); Serial.println("' to DoorOpMgr for entry.");
                    door_op_request_entry(complete_code_from_buffer); // Chiamata alla funzione del door_operation_manager
                } else {
                    Serial.println("IRInputLogic: ERROR - IR buffer empty after temp display!");
                }
                current_display_code_ir = "";
                ir_manager_reset_code_buffer();
                current_ir_input_state = LocalIRInputState::WAITING_FOR_DIGIT;
            }
            break;
    }
}