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
    String  reason; // ragione fornita da l'API Raison fournie par l'API.
    ApiResponse() : network_success(false), http_code(0), action_success(false) {
    }
};

// Struttura per i parametri recuperati dall'API
struct ApiSystemSettings {
    bool   fetch_success;
    int    max_attempts;
    int    code_validity_seconds;
    // int    code\_length; // Decommenta se gestisci anche la lunghezza del codice


    // Valori predefiniti usati se il recupero fallisce o prima del recupero
    ApiSystemSettings() : fetch_success(false), max_attempts(3), code_validity_seconds(300) {}
};


/**
 * @brief Tenta di eseguire un'azione "door_open" tramite l'API inviando un codice.
 * @param user_entered_code Il codice inserito dall'utente da convalidare.
 * @return Una struttura ApiResponse contenente il risultato della richiesta HTTP.
 */
ApiResponse api_attempt_door_open(String user_entered_code);

/**
 * @brief Notifica all'API che si è verificato un evento di chiusura della porta.
 * @param code_used_for_entry Il codice che era stato utilizzato per l'ingresso precedente.
 * @return  Una struttura ApiResponse contenente il risultato della richiesta HTTP.
 */
ApiResponse api_notify_door_close(String code_used_for_entry);

/**
 * @brief  Invia un'allerta all'API.
 * @param alert_type  Il tipo di allerta (es: "failed_code_attempts").
 * @param alert_message Il messaggio descrittivo dell'allerta.
 * @param alert_severity  La gravità dell'allerta (es: "medium", "high").
 * @return Una struttura ApiResponse contenente il risultato della richiesta HTTP.
 */
ApiResponse api_send_alert(String alert_type, String alert_message, String alert_severity);

/**
 * @brief  Recupera i parametri di configurazione del sistema dall'API.
 * @return Una struttura ApiSystemSettings contenente i parametri.
 */
ApiSystemSettings api_get_system_settings();

#endif //API_CLIENT_H