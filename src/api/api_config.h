 //
// Created by lozio on 19/05/25.
//

#ifndef API_CONFIG_H
#define API_CONFIG_H

// --- PARAMETRI DEL SERVER API ---
#define API_HOST "192.168.111.78" //
#define API_PORT 5000 //

// --- Endpoints  dell'API ---
#define API_ENDPOINT_ACCESS "/api/access" //
#define API_ENDPOINT_ALERT  "/api/alert"  //
//  Endpoint per le impostazioni (già utilizzato da api\_client.cpp ma non definito qui)
#define API_ENDPOINT_SETTINGS "/api/settings"


// --- parametri della richiesta HTTP ---
// Tempo massimo di attesa per una risposta dall'API (in millisecondi)
#define API_REQUEST_TIMEOUT_MS 10000 //

// NUOVO: Identificativo dell'agente per le richieste API
#define API_AGENT_NAME "lozio"

#endif //API_CONFIG_H
