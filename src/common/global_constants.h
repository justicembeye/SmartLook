//
// Created by lozio on 18/05/25.
//

#ifndef CONFIG_H
#define CONFIG_H



#define SERIAL_BAUD_RATE 115200

// delay per i task
const unsigned long LED_BLINK_INTERVAL_MS = 1000; // 1 secondo
const unsigned long BUTTON_READ_INTERVAL_MS = 50;   // Leggere il pulsante ogni 50 ms (per il debounce)

#endif //CONFIG_H
