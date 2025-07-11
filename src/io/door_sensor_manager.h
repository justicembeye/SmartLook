//
// Created by lozio on 20/05/25.
//

#ifndef DOOR_SENSOR_MANAGER_H
#define DOOR_SENSOR_MANAGER_H

// Inizializza il gestore dei sensori della porta (configura i pin)
void door_sensor_manager_setup();

// Verifica se il sensore indica che la porta è completamente aperta
// Restituisce true se la porta è rilevata come aperta, false altrimenti.
bool door_sensor_is_door_fully_opened();

// Verifica se il sensore indica che la porta è completamente chiusa
// Restituisce true se la porta è rilevata come chiusa, false altrimenti.
bool door_sensor_is_door_fully_closed();



#endif //DOOR_SENSOR_MANAGER_H
