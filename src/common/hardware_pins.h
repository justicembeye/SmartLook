// SmartLook/src/common/hardware_pins.h
// Mis à jour le 27/05/2025 en fonction de la configuration utilisateur et du pinout ABX00083.

#ifndef HARDWARE_PINS_H
#define HARDWARE_PINS_H

#include <Arduino.h> // Inclus pour les types comme uint8_t et les alias de broches (A0, D5, etc.)

// --- Écran LCD I2C ---
const uint8_t LCD_I2C_ADDRESS = 0x27; // Adresse I2C de l'écran.
// Broches I2C par défaut sur Arduino Nano ESP32 (ABX00083):
// SDA: A4 (sérigraphié) = GPIO11
// SCL: A5 (sérigraphié) = GPIO12
// Ces broches (A4, A5) doivent rester libres pour l'écran I2C.

// --- Communication ---
const uint8_t IR_RECEIVER_PIN = D5; // D5 sérigraphié (GPIO8)

// --- Entrées Capteurs & Boutons ---
const uint8_t EXIT_BUTTON_PIN = D10; // D10 sérigraphié (GPIO21)
const uint8_t DOOR_SENSOR_OPENED_PIN = D6;  // D6 sérigraphié (GPIO9)
const uint8_t DOOR_SENSOR_CLOSED_PIN = D7;  // D7 sérigraphié (GPIO10)

// --- Contrôle Moteur ---
const uint8_t MOTOR_L293D_IN1_PIN = D12; // D12 sérigraphié (GPIO47/CIPO) - Utilisable comme GPIO
const uint8_t MOTOR_L293D_IN2_PIN = D11; // D11 sérigraphié (GPIO38/COPI) - Utilisable comme GPIO

// --- Feedback Utilisateur (LEDs & Buzzer) ---
// Utilisation des labels sérigraphiés A0-A3 pour clarté.
// A0 = GPIO1, A1 = GPIO2 (D2), A2 = GPIO3, A3 = GPIO4
const uint8_t RGB_LED_RED_PIN   = A0; // A0 sérigraphié (GPIO1)
const uint8_t RGB_LED_GREEN_PIN = A1; // A1 sérigraphié (GPIO2, alias D2)
const uint8_t RGB_LED_BLUE_PIN  = A2; // A2 sérigraphié (GPIO3)
const uint8_t BUZZER_PIN        = A3; // A3 sérigraphié (GPIO4)

// Récapitulatif des broches sérigraphiées utilisées par ce projet :
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
// A4  : SDA pour LCD I2C
// A5  : SCL pour LCD I2C

// Broches anciennement utilisées pour LCD parallèle (non pertinentes pour cette configuration)
// const uint8_t FORMER_LCD_RS = 12;
// const uint8_t FORMER_LCD_EN = 11;
// const uint8_t FORMER_LCD_D4 = 4;
// const uint8_t FORMER_LCD_D5 = 3;
// const uint8_t FORMER_LCD_D6 = 8;
// const uint8_t FORMER_LCD_D7 = 9;

#endif //HARDWARE_PINS_H