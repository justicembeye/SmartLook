// SmartLook/src/common/hardware_pins.h
// Mis à jour le 27/05/2025 en fonction de la configuration utilisateur et du pinout ABX00083.

#ifndef HARDWARE_PINS_H
#define HARDWARE_PINS_H

#include <Arduino.h>

// --- Schermo LCD I2C ---
const uint8_t LCD_I2C_ADDRESS = 0x27; // Indirizzo I2C dello schermo.

// Pin I2C di default su Arduino Nano ESP32:
// SDA: A4 --> GPIO11
// SCL: A5 --> GPIO12

// --- Comunicazione ---
const uint8_t IR_RECEIVER_PIN = D5;

// --- Ingressi Sensori & Pulsanti ---
const uint8_t EXIT_BUTTON_PIN = D10;
const uint8_t DOOR_SENSOR_OPENED_PIN = D6;
const uint8_t DOOR_SENSOR_CLOSED_PIN = D7;

// --- Controllo Motore ---
const uint8_t MOTOR_L293D_IN1_PIN = D12;
const uint8_t MOTOR_L293D_IN2_PIN = D11;

// --- Feedback Utente (LEDs & Buzzer) ---
const uint8_t RGB_LED_RED_PIN   = A0;
const uint8_t RGB_LED_GREEN_PIN = A1;
const uint8_t RGB_LED_BLUE_PIN  = A2;
const uint8_t BUZZER_PIN        = A3;

// Riepilogo dei pin serigrafati utilizzati da questo progetto:

// D5  : IR_RECEIVER_PIN
// D6  : DOOR_SENSOR_OPENED_PIN
// D7  : DOOR_SENSOR_CLOSED_PIN
// D10 : EXIT_BUTTON_PIN
// D11 : MOTOR_L293D_IN2_PIN
// D12 : MOTOR_L293D_IN1_PIN
// A0  : RGB_LED_RED_PIN
// A1  : RGB_LED_GREEN_PIN
// A2  : RGB_LED_BLUE_PIN
// A3  : BUZZER_PIN
// A4  : SDA per LCD I2C
// A5  : SCL per LCD I2C

#endif //HARDWARE_PINS_H