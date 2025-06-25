//
// Created by lozio on 19/05/25.
//

#ifndef API_CLIENT_H
#define API_CLIENT_H
#include <WString.h>


struct ApiResponse {
    bool network_success;
    long http_code;
    String payload;
    bool action_success;
    String  reason; // ragione fornita da l'API.
    ApiResponse() : network_success(false), http_code(0), action_success(false) {
    }
};

// Struttura per i parametri recuperati dall'API
struct ApiSystemSettings {
    bool   fetch_success;
    int    max_attempts;
    int    code_validity_seconds;

    // Valori predefiniti usati se il recupero fallisce o prima del recupero
    ApiSystemSettings() : fetch_success(false), max_attempts(3), code_validity_seconds(300) {}
};


ApiResponse api_attempt_door_open(String user_entered_code);

ApiResponse api_notify_door_close(String code_used_for_entry);


ApiResponse api_send_alert(String alert_type, String alert_message, String alert_severity);


ApiSystemSettings api_get_system_settings();

#endif //API_CLIENT_H