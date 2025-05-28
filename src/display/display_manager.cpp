// SmartLook/src/display/display_manager.cpp
// (Utilisez la dernière version fournie qui utilise lcd.init() et lcd.backlight())
// Les références à LCD_COLS, LCD_ROWS, et LCD_I2C_ADDRESS
// proviendront des constantes et de hardware_pins.h.

#include <Arduino.h>
#include "common/hardware_pins.h"       // Pour LCD_I2C_ADDRESS
#include "display_manager.h"
#include "../wifi/wifi_manager.h"       // Pour wifi_is_connected()

#include <LiquidCrystal_I2C.h>

// Constantes pour l'écran
const int LCD_COLS = 16;
const int LCD_ROWS = 2;

// Déclaration de l'objet LiquidCrystal_I2C
static LiquidCrystal_I2C lcd(LCD_I2C_ADDRESS, LCD_COLS, LCD_ROWS);

// Définitions pour les caractères personnalisés
byte char_cadenas_verrouille[8] = { /* ... data ... */ };
byte char_cadenas_ouvert[8] = { /* ... data ... */ };
byte char_wifi_connecte[8] = { /* ... data ... */ };
byte char_wifi_erreur[8] = { /* ... data ... */ };
byte char_alerte[8] = { /* ... data ... */ };

const uint8_t CHAR_IDX_LOCKED    = 0;
const uint8_t CHAR_IDX_UNLOCKED  = 1;
const uint8_t CHAR_IDX_WIFI_OK   = 2;
const uint8_t CHAR_IDX_WIFI_ERR  = 3;
const uint8_t CHAR_IDX_ALERT     = 4;

void display_setup() {
    lcd.init();
    lcd.backlight();
    Serial.println("Display Manager: LCD I2C Initialized (init + backlight).");

    lcd.createChar(CHAR_IDX_LOCKED, char_cadenas_verrouille);
    lcd.createChar(CHAR_IDX_UNLOCKED, char_cadenas_ouvert);
    lcd.createChar(CHAR_IDX_WIFI_OK, char_wifi_connecte);
    lcd.createChar(CHAR_IDX_WIFI_ERR, char_wifi_erreur);
    lcd.createChar(CHAR_IDX_ALERT, char_alerte);

    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("SmartCadenas");
    lcd.setCursor(0, 1);
    lcd.print("Demarrage...");
    delay(1500);
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Code: ");
    display_update_wifi_status_icon();
    lcd.setCursor(6,0);
}

void display_update_input_code(const String& current_code_digits) {
    lcd.setCursor(6, 0);
    lcd.print(current_code_digits);
    unsigned int start_clear_pos = 6 + current_code_digits.length();
    for (unsigned int i = start_clear_pos; i < LCD_COLS; i++) {
        lcd.setCursor(i, 0);
        lcd.print(" ");
    }
    lcd.setCursor(6 + current_code_digits.length(), 0);
}

void display_show_message(const String& message, int line, bool clear_previous_line_content) {
    if (line < 0 || line >= LCD_ROWS) return;
    lcd.setCursor(0, line);
    if (clear_previous_line_content) {
        for (int i = 0; i < LCD_COLS; i++) lcd.print(" ");
        lcd.setCursor(0, line);
    }
    lcd.print(message.substring(0, LCD_COLS));
}

void display_clear_all() {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Code: ");
    display_update_wifi_status_icon();
    lcd.setCursor(6,0);
}

void display_clear_line(int line) {
    if (line < 0 || line >= LCD_ROWS) return;
    lcd.setCursor(0, line);
    for (int i = 0; i < LCD_COLS; i++) lcd.print(" ");
    if (line == 0) {
        lcd.setCursor(0,0);
        lcd.print("Code: ");
        lcd.setCursor(6,0);
    }
}

void display_write_custom_char(uint8_t char_index) {
    lcd.write(char_index);
}

void display_update_wifi_status_icon() {
    lcd.setCursor(LCD_COLS - 1, 1);
    if (wifi_is_connected()) {
        lcd.write(CHAR_IDX_WIFI_OK);
    } else {
        lcd.write(CHAR_IDX_WIFI_ERR);
    }
    // Ne pas repositionner le curseur ici, laisser l'appelant le gérer.
}