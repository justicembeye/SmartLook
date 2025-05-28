//
// Created by lozio on 20/05/25.
//

#ifndef DOOR_MOTOR_MANAGER_H
#define DOOR_MOTOR_MANAGER_H

// Initialise le gestionnaire du moteur (configure les broches)
void door_motor_manager_setup();

// Fait tourner le moteur pour ouvrir la porte
void door_motor_open();

// Fait tourner le moteur pour fermer la porte
void door_motor_close();

// Arrête le moteur (freinage)
void door_motor_stop();

#endif //DOOR_MOTOR_MANAGER_H
