// SmartLook/src/api/api_client.cpp

#include "api_client.h"      // Nos déclarations
#include "api_config.h"      // Nos configurations API (API_HOST, API_PORT, etc.)
#include "../wifi/wifi_manager.h" // Pour utiliser wifi_is_connected()


#include <HTTPClient.h>
#include <ArduinoJson.h>
#include <Arduino.h> // Pour Serial, String, etc.


ApiResponse api_attempt_door_open(String user_entered_code) {
    ApiResponse response;
    if (!wifi_is_connected()) { //
        Serial.println("API_CLIENT (door_open): Erreur - Wi-Fi non connecte. Impossible d'envoyer la requete api_attempt_door_open.");
        response.network_success = false;
        response.reason = "Wi-Fi disconnected";
        return response;
    }

    const String url = String("http://") + API_HOST + ":" + String(API_PORT) + API_ENDPOINT_ACCESS;
    Serial.println("API_CLIENT (door_open): 🌐 Url : " + url);

    // Augmentation de la capacité si l'agent est ajouté, sinon 128 peut suffire.
    // Pour {"event":"door_open","code":"XXXX", "agent":"ESP32_SmartCadenas_01"}
    // Longueur approx: 20 + 7 + 7 + 5 + 7 + 24 + 5 = 75. Donc 128 est ok.
    const int capacity = 128;
    DynamicJsonDocument request_doc(capacity);
    request_doc["event"] = "door_open";
    request_doc["code"] = user_entered_code;
    request_doc["agent"] = API_AGENT_NAME; // NOUVEL AJOUT

    String json_request_body;
    serializeJson(request_doc, json_request_body);
    Serial.println("API_CLIENT (door_open): Contenu de la requête HTTP : " + json_request_body);

    HTTPClient http;
    http.begin(url);
    http.addHeader("Content-Type", "application/json");
    http.setTimeout(API_REQUEST_TIMEOUT_MS); //

    Serial.println("API_CLIENT (door_open): Envoi de la requete POST...");
    const int httpCode = http.POST(json_request_body);
    response.http_code = httpCode;

    if (httpCode > 0) {
        response.network_success = true;
        response.payload = http.getString();
        Serial.println("API_CLIENT (door_open): code de statut HTTP : " + String(httpCode) + " - Payload: " + response.payload);

        if (!response.payload.isEmpty()) {
            DynamicJsonDocument response_doc(256);
            DeserializationError error = deserializeJson(response_doc, response.payload);
            if (!error) {
                if (response_doc.containsKey("event_status")) {
                    String event_status_str = response_doc["event_status"].as<String>();
                    Serial.println("API_CLIENT (door_open): Parsed event_status -> " + event_status_str);
                    if (event_status_str.equals("success")) {
                        response.action_success = true;
                    } else {
                        response.action_success = false;
                    }
                } else {
                    response.action_success = false;
                    Serial.println("API_CLIENT (door_open): Cle 'event_status' manquante dans la reponse JSON.");
                }

                if (response_doc.containsKey("reason") && !response_doc["reason"].isNull()) {
                    response.reason = response_doc["reason"].as<String>();
                } else if (!response.action_success && response.reason.isEmpty()) {
                    response.reason = "API indicated failure without specific reason";
                } else if (response.action_success) {
                    response.reason = ""; // Pas de raison si succès
                }
            } else {
                Serial.print("API_CLIENT (door_open): Erreur de deserialisation du payload : ");
                Serial.println(error.c_str());
                response.action_success = false;
                response.reason = "JSON parsing failed: " + String(error.c_str());
            }
        } else if (httpCode == HTTP_CODE_CREATED || httpCode == HTTP_CODE_OK) {
             response.action_success = false; // Un succès avec payload vide peut être un problème si on attend des infos
             response.reason = "API returned success HTTP code but empty payload.";
        }

        // Si le code HTTP n'est pas un succès et qu'aucune raison n'a été extraite du JSON
        if ((httpCode != HTTP_CODE_CREATED && httpCode != HTTP_CODE_OK) && response.reason.isEmpty()) {
            response.reason = "API responded with unexpected HTTP code: " + String(httpCode);
            response.action_success = false; // Assurer que action_success est false
        }

    } else {
        response.network_success = false;
        response.payload = http.errorToString(httpCode).c_str(); // Conserver l'erreur technique
        response.reason = response.payload; // Mettre l'erreur technique comme raison
        Serial.print("API_CLIENT (door_open): Erreur de la requete POST, code HTTPClient : ");
        Serial.println(httpCode);
        Serial.print("API_CLIENT (door_open): Message d'erreur HTTPClient : ");
        Serial.println(response.reason);
    }

    http.end();
    return response;
}


ApiResponse api_notify_door_close(String code_used_for_entry) {
    ApiResponse response;
    if (!wifi_is_connected()) {
        Serial.println("API_CLIENT (door_close): Erreur - Wi-Fi non connecte. Impossible d'envoyer la requete api_notify_door_close.");
        response.network_success = false;
        response.reason = "Wi-Fi disconnected";
        return response;
    }

    const String url = String("http://") + API_HOST + ":" + String(API_PORT) + API_ENDPOINT_ACCESS;
    Serial.println("API_CLIENT (door_close): 🌐 Url : " + url);

    // Capacité pour {"event":"door_close","code":"XXXX" (ou _LBE_ ou vide), "agent":"ESP32_SmartCadenas_01"}
    // Longueur approx: 20 + 7 + 13 + 7 + 5 + 7 + 24 + 5 = 88. 128 est ok.
    int capacity = 128;
    DynamicJsonDocument request_doc(capacity);
    request_doc["event"] = "door_close";
    request_doc["code"] = code_used_for_entry;
    request_doc["agent"] = API_AGENT_NAME; // NOUVEL AJOUT

    String json_request_body;
    serializeJson(request_doc, json_request_body);
    Serial.println("API_CLIENT (door_close): Contenu de la requête HTTP : " + json_request_body);

    HTTPClient http;
    http.begin(url);
    http.addHeader("Content-Type", "application/json");
    http.setTimeout(API_REQUEST_TIMEOUT_MS);

    Serial.println("API_CLIENT (door_close): Envoi de la requete POST...");
    const int httpCode = http.POST(json_request_body);
    response.http_code = httpCode;

    if (httpCode > 0) {
        response.network_success = true;
        response.payload = http.getString();
        Serial.println("API_CLIENT (door_close): code de statut HTTP : " + String(httpCode) + " - Payload: " + response.payload);

        if (!response.payload.isEmpty()) {
            DynamicJsonDocument response_doc(256); // La réponse peut contenir une raison
            DeserializationError error = deserializeJson(response_doc, response.payload);
            if (!error) {
                if (response_doc.containsKey("event_status")) {
                    String event_status_str = response_doc["event_status"].as<String>();
                    Serial.println("API_CLIENT (door_close): Parsed event_status -> " + event_status_str);
                    // L'API peut renvoyer "success" ou "warning" pour un door_close
                    if (event_status_str.equals("success") || event_status_str.equals("warning")) {
                        response.action_success = true;
                        Serial.println("API_CLIENT (door_close): Notification de fermeture traitee avec succes/warning par l'API.");
                    } else {
                        response.action_success = false;
                        Serial.print("API_CLIENT (door_close): L'API a retourne un event_status inattendu: ");
                        Serial.println(event_status_str);
                    }
                } else {
                    response.action_success = false;
                    Serial.println("API_CLIENT (door_close): Cle 'event_status' manquante dans la reponse JSON de l'API.");
                }

                if (response_doc.containsKey("reason") && !response_doc["reason"].isNull()) {
                    response.reason = response_doc["reason"].as<String>();
                } else if (!response.action_success && response.reason.isEmpty()) {
                    response.reason = "API indicated failure/unexpected status without specific reason for door_close";
                } else if (response.action_success) {
                     // Garder la raison si l'API en fournit une même en cas de succès (ex: warning)
                     response.reason = response_doc["reason"].as<String>();
                     if (response.reason.isEmpty() || response_doc["reason"].isNull()) response.reason = "Door close event processed";
                }
            } else {
                Serial.print("API_CLIENT (door_close): Echec du parsing de la reponse JSON de l'API: ");
                Serial.println(error.c_str());
                response.action_success = false;
                response.reason = "JSON parsing failed for door_close response: " + String(error.c_str());
            }
        } else if (httpCode == HTTP_CODE_CREATED || httpCode == HTTP_CODE_OK) {
             response.action_success = false; // Un succès avec payload vide est suspect si on attend un statut/raison
             response.reason = "API returned success HTTP code but empty payload for door_close.";
        }

        if ((httpCode != HTTP_CODE_CREATED && httpCode != HTTP_CODE_OK) && response.reason.isEmpty()) {
            response.reason = "API responded with unexpected HTTP code for door_close: " + String(httpCode);
            response.action_success = false;
        }

    } else {
        response.network_success = false;
        response.payload = http.errorToString(httpCode).c_str();
        response.reason = response.payload;
        Serial.print("API_CLIENT (door_close): Erreur de la requete POST, code HTTPClient : ");
        Serial.println(httpCode);
        Serial.print("API_CLIENT (door_close): Message d'erreur HTTPClient : ");
        Serial.println(response.reason);
    }

    http.end();
    return response;
}

ApiResponse api_send_alert(String alert_type, String alert_message, String alert_severity) {
    ApiResponse response;
    if (!wifi_is_connected()) {
        Serial.println("API_CLIENT (alert): Erreur - Wi-Fi non connecte. Impossible d'envoyer la requete api_send_alert.");
        response.network_success = false;
        response.reason = "Wi-Fi disconnected";
        return response;
    }

    const String url = String("http://") + API_HOST + ":" + String(API_PORT) + API_ENDPOINT_ALERT;
    Serial.println("API_CLIENT (alert): 🌐 Url : " + url);

    // Capacité pour {"type":"...", "message":"...", "severity":"...", "agent":"..."}
    // Ex: {"type":"multiple_failed_attempts","message":"3 tentatives code echouees","severity":"high", "agent":"ESP32_SmartCadenas_01"}
    // Longueur approx: 20 + 7 + 26 + 12 + 30 + 13 + 7 + 7 + 24 + 5 = 151. 256 est sûr.
    int capacity = 256;
    DynamicJsonDocument request_doc(capacity);
    request_doc["type"] = alert_type;
    request_doc["message"] = alert_message;
    request_doc["severity"] = alert_severity;
    request_doc["agent"] = API_AGENT_NAME; // NOUVEL AJOUT

    String json_request_body;
    serializeJson(request_doc, json_request_body);
    Serial.println("API_CLIENT (alert): Contenu de la requête HTTP : " + json_request_body);

    HTTPClient http;
    http.begin(url);
    http.addHeader("Content-Type", "application/json");
    http.setTimeout(API_REQUEST_TIMEOUT_MS);

    Serial.println("API_CLIENT (alert): Envoi de la requete POST...");
    const int httpCode = http.POST(json_request_body);
    response.http_code = httpCode;

    if (httpCode > 0) {
        response.network_success = true;
        response.payload = http.getString();
        Serial.println("API_CLIENT (alert): code de statut HTTP : " + String(httpCode) + " - Payload: " + response.payload);

        if (!response.payload.isEmpty()) {
            DynamicJsonDocument response_doc(256); // La réponse peut contenir alert_id, timestamp
            DeserializationError error = deserializeJson(response_doc, response.payload);
            if (!error) {
                if (response_doc.containsKey("status")) { // L'API Flask renvoie "status"
                    String status_str = response_doc["status"].as<String>();
                    Serial.println("API_CLIENT (alert): Parsed status -> " + status_str);
                    if (status_str.equals("alert_created")) {
                        response.action_success = true;
                        Serial.println("API_CLIENT (alert): Alerte envoyee avec succes a l'API.");
                        if (response_doc.containsKey("alert_id")) {
                            response.reason = "Alert created with ID: " + response_doc["alert_id"].as<String>();
                        } else if (response_doc.containsKey("timestamp")) { // Fallback si alert_id n'est pas là
                            response.reason = "Alert created at: " + response_doc["timestamp"].as<String>();
                        } else {
                             response.reason = "Alert created successfully";
                        }
                    } else {
                        response.action_success = false;
                        Serial.print("API_CLIENT (alert): L'API a retourne un statut inattendu pour l'alerte: ");
                        Serial.println(status_str);
                        response.reason = "API returned unexpected status for alert: " + status_str;
                    }
                } else {
                     response.action_success = false;
                     response.reason = "Cle 'status' manquante dans la reponse JSON de l'alerte.";
                     Serial.println("API_CLIENT (alert): " + response.reason);
                }
            } else {
                Serial.print("API_CLIENT (alert): Echec du parsing de la reponse JSON de l'alerte: ");
                Serial.println(error.c_str());
                response.action_success = false;
                response.reason = "JSON parsing failed for alert response: " + String(error.c_str());
            }
        } else if (httpCode == HTTP_CODE_CREATED || httpCode == HTTP_CODE_OK) {
             response.action_success = false; // Payload vide suspect pour une création d'alerte
             response.reason = "API returned success HTTP code but empty payload for alert.";
        }

        if ((httpCode != HTTP_CODE_CREATED && httpCode != HTTP_CODE_OK) && response.reason.isEmpty()) {
            response.reason = "API responded with unexpected HTTP code for alert: " + String(httpCode);
            response.action_success = false;
        }

    } else {
        response.network_success = false;
        response.payload = http.errorToString(httpCode).c_str();
        response.reason = response.payload;
        Serial.print("API_CLIENT (alert): Erreur de la requete POST, code HTTPClient : ");
        Serial.println(httpCode);
        Serial.print("API_CLIENT (alert): Message d'erreur HTTPClient : ");
        Serial.println(response.reason);
    }

    http.end();
    return response;
}

// NOUVELLE FONCTION - api_get_system_settings() déjà présente et correcte, pas besoin d'y ajouter "agent"
ApiSystemSettings api_get_system_settings() { //
    ApiSystemSettings settings_response;
    if (!wifi_is_connected()) {
        Serial.println("API_CLIENT (Settings): Erreur - Wi-Fi non connecté. Impossible de récupérer les paramètres.");
        settings_response.fetch_success = false;
        return settings_response;
    }

    HTTPClient http;
    String url = String("http://") + API_HOST + ":" + String(API_PORT) + API_ENDPOINT_SETTINGS; // Utilisation de la define
    Serial.println("API_CLIENT (Settings): 🌐 Url : " + url);

    http.begin(url);
    http.setTimeout(API_REQUEST_TIMEOUT_MS);

    int httpCode = http.GET();
    settings_response.fetch_success = false;

    if (httpCode == HTTP_CODE_OK) {
        String payload = http.getString();
        Serial.println("API_CLIENT (Settings): Réponse reçue, Payload: " + payload);

        DynamicJsonDocument doc(256);
        DeserializationError error = deserializeJson(doc, payload);

        if (!error) {
            if (doc.containsKey("max_attempts") && doc.containsKey("code_validity")) { //
                settings_response.max_attempts = doc["max_attempts"].as<int>();
                settings_response.code_validity_seconds = doc["code_validity"].as<int>();
                settings_response.fetch_success = true;
                Serial.println("API_CLIENT (Settings): Paramètres système récupérés et parsés avec succès.");
            } else {
                Serial.println("API_CLIENT (Settings): Erreur - Clés JSON 'max_attempts' ou 'code_validity' manquantes dans la réponse.");
            }
        } else {
            Serial.print("API_CLIENT (Settings): Erreur de désérialisation JSON du payload des paramètres: ");
            Serial.println(error.c_str());
        }
    } else {
        Serial.print("API_CLIENT (Settings): Erreur de la requête GET pour les paramètres, code HTTP: ");
        Serial.println(httpCode);
        if (httpCode > 0) {
             Serial.print("API_CLIENT (Settings): Message d'erreur API: "); Serial.println(http.getString());
        } else {
            Serial.print("API_CLIENT (Settings): Message d'erreur HTTPClient: "); Serial.println(http.errorToString(httpCode));
        }
    }

    http.end();
    return settings_response;
}