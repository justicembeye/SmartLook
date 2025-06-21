// SmartLook/src/display/display_manager.cpp
// (Version avec corrections intégrées)

#include <Arduino.h>
#include "common/hardware_pins.h"       // Pour LCD_I2C_ADDRESS
#include "display_manager.h"
#include "../wifi/wifi_manager.h"       // Pour wifi_is_connected()

#include <LiquidCrystal_I2C.h>

// costanti per il diplay
const int LCD_COLS = 16;
const int LCD_ROWS = 2;

// dichiarazione dell'oggetto liquidCrystal_I2C
static LiquidCrystal_I2C lcd(LCD_I2C_ADDRESS, LCD_COLS, LCD_ROWS);

// Definizione per i carateri personalizati
byte char_cadenas_verrouille[8] = {0x0E, 0x11, 0x11, 0x1F, 0x1B, 0x1B, 0x1F, 0x00};
byte char_cadenas_ouvert[8] = {0x0E, 0x11, 0x01, 0x1F, 0x1B, 0x1B, 0x1F, 0x00};
byte char_wifi_connecte[8] = {0x00, 0x0E, 0x1F, 0x0E, 0x04, 0x0A, 0x11, 0x00};
byte char_wifi_erreur[8] = {0x00, 0x0E, 0x1F, 0x00, 0x15, 0x0A, 0x15, 0x0A};
byte char_alerte[8] = {0x00, 0x04, 0x0E, 0x1F, 0x1F, 0x0E, 0x04, 0x00};

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
    // 1. prima cancelliamo le zone dove il codice è abitualmentte scritto per evitare ripetizione.
    lcd.setCursor(6, 0); // la posizione direttamente dopo il "code : "
    for (int i = 6; i < LCD_COLS; i++) {
        lcd.print(" ");
    }

    // 2. Dopo torniamo a l'inizio della zona e scriviamo il nuovo codice.
    lcd.setCursor(6, 0);
    lcd.print(current_code_digits);
    // IL cursore è ora posto subito dopo l'ultima cifra , pronto a ricevere il prossimo.
}


void display_show_message(const String& message, int line, bool clear_previous_line_content) {
    if (line < 0 || line >= LCD_ROWS) return;

    // pozioniamo il cursore a l'inizio della linea desiderata.
    lcd.setCursor(0, line);

    //scriviamo il nuovo messaggio( dobbiamo tenere conto che non deve superare la larghezza dello schermo).
    lcd.print(message.substring(0, LCD_COLS));
     // cancelliamo il resto delle righe  per evitare carateri fantasmi se il nuovo messaggio è piu corto di quello precedente

    for (int i = message.length(); i < LCD_COLS; i++) {
        lcd.print(" ");
    }
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
    // non posizionare il cursore qua . lasciamo il chiamante gestire questo
}