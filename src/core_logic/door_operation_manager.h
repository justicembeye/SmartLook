//
// Created by lozio on 25/05/25.
//

#ifndef DOOR_OPERATION_MANAGER_H
#define DOOR_OPERATION_MANAGER_H

#include <Arduino.h> // Pour String, si utilisé dans les fonctions publiques

// États pour la logique d'opération de la porte
enum class DoorOpState {
    IDLE_CLOSED,
    IDLE_OPEN,
    IDLE_AJAR,
    PENDING_API_VALIDATION,
    MOTOR_OPENING,
    MOTOR_CLOSING,
    MOTOR_TIMEOUT,
    API_ERROR,
    LOCKED_OUT // NOUVEL ÉTAT pour le verrouillage local
};

void door_op_manager_setup();
void door_op_manager_task();

void door_op_request_entry(const String& ir_code);

#endif //DOOR_OPERATION_MANAGER_H