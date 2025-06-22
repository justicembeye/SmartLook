// SmartLook/src/core_logic/door_operation_manager.h
// Version finale et complète

#ifndef DOOR_OPERATION_MANAGER_H
#define DOOR_OPERATION_MANAGER_H

#include <Arduino.h>

// Énumération de classe pour les états de la machine d'états.
enum class DoorOpState {
    IDLE_CLOSED,
    IDLE_OPEN,
    IDLE_AJAR,
    PENDING_API_VALIDATION,
    MOTOR_OPENING,
    MOTOR_CLOSING,
    MOTOR_TIMEOUT,
    API_ERROR,
    LOCKED_OUT,
    INVALID_CODE_INFO // État pour gérer l'affichage temporaire d'un code invalide
};

void door_op_manager_setup();

void door_op_manager_task();

void door_op_request_entry(const String& ir_code);

#endif //DOOR_OPERATION_MANAGER_H