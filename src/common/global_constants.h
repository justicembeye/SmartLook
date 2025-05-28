//
// Created by lozio on 18/05/25.
//

#ifndef CONFIG_H
#define CONFIG_H



#define SERIAL_BAUD_RATE 115200

// Délais pour les tâches (exemples)
const unsigned long LED_BLINK_INTERVAL_MS = 1000; // 1 seconde
const unsigned long BUTTON_READ_INTERVAL_MS = 50;   // Lire le bouton toutes les 50ms (pour le dérebond)

#endif //CONFIG_H
