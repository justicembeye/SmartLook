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
    bool action_success; // L'action métier de l'API a-t-elle réussi ?
    String  reason; // Raison fournie par l'API.
    ApiResponse() : network_success(false), http_code(0), action_success(false) {
    }
};

// NOUVEAU: Structure pour les paramètres récupérés de l'API
struct ApiSystemSettings {
    bool   fetch_success;
    int    max_attempts;
    int    code_validity_seconds;
    // int    code_length; // Décommentez si vous gérez aussi la longueur du code

    // Valeurs par défaut utilisées si la récupération échoue ou avant la récupération
    ApiSystemSettings() : fetch_success(false), max_attempts(3), code_validity_seconds(300) {}
};


/**
 * @brief Tente d'effectuer une action "door_open" via l'API en envoyant un code.
 * @param user_entered_code Le code saisi par l'utilisateur à valider.
 * @return Une structure ApiResponse contenant le résultat de la requête HTTP.
 */
ApiResponse api_attempt_door_open(String user_entered_code);

/**
 * @brief Notifie l'API qu'un événement de fermeture de porte a eu lieu.
 * @param code_used_for_entry Le code qui avait été utilisé pour l'entrée précédente.
 * @return Une structure ApiResponse contenant le résultat de la requête HTTP.
 */
ApiResponse api_notify_door_close(String code_used_for_entry);

/**
 * @brief Envoie une alerte à l'API.
 * @param alert_type Le type d'alerte (ex: "failed_code_attempts").
 * @param alert_message Le message descriptif de l'alerte.
 * @param alert_severity La gravité de l'alerte (ex: "medium", "high").
 * @return Une structure ApiResponse contenant le résultat de la requête HTTP.
 */
ApiResponse api_send_alert(String alert_type, String alert_message, String alert_severity);

/**
 * @brief NOUVEAU: Récupère les paramètres de configuration du système depuis l'API.
 * @return Une structure ApiSystemSettings contenant les paramètres.
 */
ApiSystemSettings api_get_system_settings();

#endif //API_CLIENT_H