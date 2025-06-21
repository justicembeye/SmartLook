 //
// Created by lozio on 19/05/25.
//

#ifndef API_CONFIG_H
#define API_CONFIG_H

// --- Paramètres du Serveur API ---
#define API_HOST "192.168.111.78" //
#define API_PORT 5000 //

// --- Endpoints de l'API (chemins) ---
#define API_ENDPOINT_ACCESS "/api/access" //
#define API_ENDPOINT_ALERT  "/api/alert"  //
// NOUVEAU: Endpoint pour les settings (déjà utilisé par api_client.cpp mais pas défini ici)
#define API_ENDPOINT_SETTINGS "/api/settings"


// --- Paramètres des Requêtes HTTP ---
// Délai d'attente maximal pour une réponse de l'API (en millisecondes)
#define API_REQUEST_TIMEOUT_MS 10000 //

// NOUVEAU: Identifiant de l'agent pour les requêtes API
#define API_AGENT_NAME "lozio"

#endif //API_CONFIG_H
