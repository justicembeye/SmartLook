// SmartLook/src/main.cpp
// (Utilisez la version révisée de mon message précédent qui n'appelle pas directement lcd.setCursor())
// Les #include et la structure générale restent les mêmes.
// Les appels à display_setup(), display_show_message(), etc. fonctionneront
// car l'interface de display_manager.h n'a pas changé.

#include <Arduino.h>
#include "common/global_constants.h"
#include "common/app_config.h"
#include "display/display_manager.h" // Notre gestionnaire d'affichage mis à jour
#include "wifi/wifi_manager.h"
#include "io/ir_receiver_manager.h"
#include "io/input_manager.h"
#include "io/door_sensor_manager.h"
#include "io/door_motor_manager.h"
#include "io/feedback_manager.h"
#include "app_logic/ir_code_input_logic.h"
#include "core_logic/door_operation_manager.h"
#include "scheduler/TaskScheduler.h"

TaskScheduler scheduler;

void update_door_sensors_task() {
    door_sensor_is_door_fully_opened();
    door_sensor_is_door_fully_closed();
}

void setup() {
    Serial.begin(SERIAL_BAUD_RATE);
    // ... (attente Serial) ...
    Serial.println("--- SmartLook System Initializing ---");

    Serial.println("Initializing Display Manager...");
    display_setup(); // Initialise LCD I2C, affiche "SmartCadenas", "Demarrage...", puis "Code: " + icône WiFi

    Serial.println("Initializing WiFi Manager & Connecting...");
    display_show_message("Connexion WiFi..", 1, true);

    wifi_connect();

    if (wifi_is_connected()) {
        display_show_message("WiFi OK", 1, true);
        Serial.println("WiFi Connected.");
    } else {
        display_show_message("WiFi Err", 1, true);
        Serial.println("WiFi Connection Failed.");
    }
    display_update_wifi_status_icon(); // Mettre à jour l'icône WiFi
    // Après les messages WiFi, ré-afficher le prompt "Code:" pour s'assurer qu'il est là
    // et que le curseur est prêt pour la saisie. display_update_input_code("") peut faire cela.
    display_update_input_code(""); // Efface les chiffres et repositionne le curseur après "Code: "

    if (wifi_is_connected()) {
        delay(1000);
        load_app_settings_from_api();
    } else {
        load_app_settings_from_api();
    }

    // Optionnel: effacer la ligne de statut WiFi après un moment
    // delay(2000);
    // display_clear_line(1);
    // display_update_wifi_status_icon(); // Rétablir l'icône si la ligne est effacée

    Serial.println("Initializing IR Receiver Manager...");
    ir_manager_setup();
    Serial.println("Initializing Input Manager (Exit Button)...");
    input_manager_setup();
    Serial.println("Initializing Door Sensor Manager...");
    door_sensor_manager_setup();
    Serial.println("Initializing Door Motor Manager...");
    door_motor_manager_setup();
    Serial.println("Initializing Feedback Manager...");
    feedback_manager_setup();
    Serial.println("Initializing Door Operation Manager...");
    door_op_manager_setup();

    Serial.println("Adding tasks to scheduler...");
    scheduler.addTask(ir_code_input_logic_task);
    scheduler.addTask(door_op_manager_task);
    scheduler.addTask(update_door_sensors_task);
    scheduler.addTask(feedback_manager_task);

    Serial.println("--- System Initialization Complete ---");
    display_show_message("Pret.", 1, true);
    // S'assurer que la ligne 0 est prête pour la saisie du code
    display_update_input_code("");
}

void loop() {
    scheduler.run();
    delay(10);
}