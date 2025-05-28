//
// Created by lozio on 20/05/25.
//

// src/io/door_sensor_manager.cpp
#include "door_sensor_manager.h"
#include "../common/hardware_pins.h" // Pour DOOR_SENSOR_OPENED_PIN, DOOR_SENSOR_CLOSED_PIN
#include <Arduino.h>                 // Pour pinMode, digitalRead, millis

const unsigned long DOOR_SENSOR_DEBOUNCE_DELAY_MS = 50; // Ajusté à 50ms

static byte stable_door_opened_state = HIGH;
static byte last_raw_door_opened_reading = HIGH;
static unsigned long last_door_opened_change_time = 0;

static byte stable_door_closed_state = HIGH;
static byte last_raw_door_closed_reading = HIGH;
static unsigned long last_door_closed_change_time = 0;

void door_sensor_manager_setup() {
    pinMode(DOOR_SENSOR_OPENED_PIN, INPUT_PULLUP);
    pinMode(DOOR_SENSOR_CLOSED_PIN, INPUT_PULLUP);

    stable_door_opened_state = digitalRead(DOOR_SENSOR_OPENED_PIN);
    last_raw_door_opened_reading = stable_door_opened_state;

    stable_door_closed_state = digitalRead(DOOR_SENSOR_CLOSED_PIN);
    last_raw_door_closed_reading = stable_door_closed_state;
    Serial.println("Door Sensor Manager: Pins initialized (with debounce).");
}

bool door_sensor_is_door_fully_opened() {
    byte current_raw_reading = digitalRead(DOOR_SENSOR_OPENED_PIN);
    if (current_raw_reading != last_raw_door_opened_reading) {
        last_door_opened_change_time = millis();
    }
    if ((millis() - last_door_opened_change_time) > DOOR_SENSOR_DEBOUNCE_DELAY_MS) {
        if (current_raw_reading != stable_door_opened_state) {
            stable_door_opened_state = current_raw_reading;
        }
    }
    last_raw_door_opened_reading = current_raw_reading;
    return (stable_door_opened_state == LOW);
}

bool door_sensor_is_door_fully_closed() {
    byte current_raw_reading = digitalRead(DOOR_SENSOR_CLOSED_PIN);
    if (current_raw_reading != last_raw_door_closed_reading) {
        last_door_closed_change_time = millis();
    }
    if ((millis() - last_door_closed_change_time) > DOOR_SENSOR_DEBOUNCE_DELAY_MS) {
        if (current_raw_reading != stable_door_closed_state) {
            stable_door_closed_state = current_raw_reading;
        }
    }
    last_raw_door_closed_reading = current_raw_reading;
    return (stable_door_closed_state == LOW);
}