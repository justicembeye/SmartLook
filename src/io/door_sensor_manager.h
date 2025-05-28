//
// Created by lozio on 20/05/25.
//

#ifndef DOOR_SENSOR_MANAGER_H
#define DOOR_SENSOR_MANAGER_H

// Initialise le gestionnaire des capteurs de porte (configure les broches)
void door_sensor_manager_setup();

// Vérifie si le capteur indique que la porte est complètement ouverte
// Retourne true si la porte est détectée comme ouverte, false sinon.
bool door_sensor_is_door_fully_opened();

// Vérifie si le capteur indique que la porte est complètement fermée
// Retourne true si la porte est détectée comme fermée, false sinon.
bool door_sensor_is_door_fully_closed();

// (Optionnel pour plus tard) Une fonction qui retourne un état plus global
// enum class DoorState { OPEN, CLOSED, MOVING, AJAR, ERROR };
// DoorState door_sensor_get_current_door_state();

#endif //DOOR_SENSOR_MANAGER_H
