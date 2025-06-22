//
// Created by lozio on 20/05/25.
//

#ifndef DOOR_MOTOR_MANAGER_H
#define DOOR_MOTOR_MANAGER_H

// Inizializza il gestore del motore (configura i pin)

void door_motor_manager_setup();

// Fa girare il motore per aprire la porta

void door_motor_open();

// Fa girare il motore per chiudere la porta

void door_motor_close();

// Ferma il motore (frenata)

void door_motor_stop();

#endif //DOOR_MOTOR_MANAGER_H
