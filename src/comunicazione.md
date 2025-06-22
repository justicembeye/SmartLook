

# Comunicazioni tra Arduino e l'API SmartLock

## 📱 Scenari di Comunicazione Semplificati

Si immagina questa comunicazione come una conversazione tra un **agente sul campo** (l'Arduino) e il suo **centro di controllo** (l'API).

### Scenario 1: Tentativo di apertura della porta

1.  **Il tecnico inserisce un codice sulla serratura.**
2.  **L'agente sul campo (Arduino) riferisce al centro di controllo:** "Sto tentando di aprire la porta con questo codice."
    - *"Centro di controllo, qui agente 7. Richiedo l'apertura con il codice 4321. Autorizzazione?"*
3.  **Il centro di controllo (API) verifica e risponde:** "Azione autorizzata" o "Azione rifiutata + motivo".
    - *"Agente 7, autorizzazione concessa. Procedere."* OPPURE
    - *"Agente 7, autorizzazione rifiutata. Motivo: codice non corretto."*
4.  **L'agente sul campo (Arduino) agisce** in base alla risposta.
    - Apre la porta e segnala il successo.
    - Rimane chiuso e segnala l'errore.

### Scenario 2: Rapporto di chiusura della porta (Uscita)

1.  **Il tecnico preme il pulsante di uscita.**
2.  **L'agente sul campo (Arduino) apre la porta e riferisce:** "La porta è stata aperta dall'interno per un'uscita."
    - *"Centro di controllo, segnalo un'uscita tramite il pulsante. Registro l'evento."*
3.  **Il centro di controllo (API) prende nota dell'informazione:** "Uscita registrata."
    - *"Ben ricevuto. Ho annotato l'uscita nel registro."*

### Scenario 3: Allarme di sicurezza

1.  **Vengono inseriti più codici errati.**
2.  **L'agente sul campo (Arduino) invia un allarme prioritario:** "Rilevata attività sospetta\!"
    - *"URGENTE, Centro di controllo\! Ho registrato 3 tentativi di accesso falliti\!"*
3.  **Il centro di controllo (API) crea un allarme:** "Allarme di sicurezza registrato."
    - *"Creato allarme ad alta priorità. L'incidente è ora tracciato."*

## 🔄 Flusso di Comunicazione Tecnico Reale

La comunicazione avviene tramite richieste `POST` in HTTP sulla rete WiFi. L'Arduino non usa `GET` per fare domande, invia direttamente rapporti di azione.

### 1\. Connessione WiFi

```
[Arduino] --- Connessione WiFi ---> [Rete locale] --- Connessione ---> [Server API]
```

- L'Arduino si connette al WiFi e conosce l'indirizzo IP del server API.

### 2\. Tentativo di accesso (Validazione + Registrazione)

L'Arduino utilizza **un unico endpoint** per tentare un'apertura.

```
[Arduino] --- POST /api/access {"event": "door_open", "code": "1234", "agent": "lozio"} ---> [API] 
[Arduino] <--- {"status": "logged", "event_status": "success"} --- [API]
oppure
[Arduino] <--- {"status": "logged", "event_status": "failed", "reason": "code_incorrect"} --- [API]
```

- Questa singola chiamata serve sia come **richiesta di validazione** sia come **prima registrazione** nei log.

### 3\. Rapporto di uscita o di chiusura

```
[Arduino] --- POST /api/access {"event": "door_close", "code": "_LBE_", "agent": "lozio"} ---> [API]
[Arduino] <--- {"status": "logged", "event_status": "success"} --- [API]
```

### 4\. Creazione di allarmi

```
[Arduino] --- POST /api/alert {"type": "multiple_failed_attempts", "message": "...", "severity": "high"} ---> [API]
[Arduino] <--- {"status": "alert_created", "alert_id": ...} --- [API]
```

- L'API risponde con lo stato e l'ID dell'allarme creato.

## 🔌 Dettagli Tecnici

### Come "parla" l'Arduino in HTTP?

1.  **Libreria WiFi**

    - Utilizzata per stabilire la connessione iniziale.

2.  **Libreria HTTPClient**

    - L'esempio reale è un `POST` con un corpo del messaggio JSON.

    <!-- end list -->

    ```cpp
    HTTPClient http;
    http.begin("http://192.168.111.78:5000/api/access"); // URL dell'endpoint
    http.addHeader("Content-Type", "application/json"); // Specificare che si invia JSON

    int httpCode = http.POST(json_request_body); // Invio del corpo della richiesta JSON

    if (httpCode > 0) {
        String payload = http.getString(); // Recupero della risposta
    }
    http.end();
    ```

3.  **Libreria ArduinoJson**

    - Essenziale per costruire la stringa JSON da inviare (`serializeJson`) e per analizzare la risposta ricevuta dal server (`deserializeJson`).

### Formato degli Scambi

#### Tentativo di accesso (`POST /api/access`)

**Richiesta dall'Arduino:**

```json
{
    "event": "door_open",
    "code": "4596",
    "agent": "lozio"
}
```

**Risposta dall'API (Successo):**

```json
{
    "status": "logged",
    "event_status": "success"
}
```

**Risposta dall'API (Fallimento):**

```json
{
    "status": "logged",
    "event_status": "failed",
    "reason": "code_incorrect"
}
```

#### Allarme (`POST /api/alert`)

**Richiesta dall'Arduino:**

```json
{
    "type": "multiple_failed_attempts",
    "message": "3 tentativi codice falliti",
    "severity": "high",
    "agent": "lozio"
}
```

**Risposta dall'API:**

```json
{
    "status": "alert_created",
    "alert_id": 2,
    "timestamp": "2025-06-21T13:43:48.026973"
}
```

## 🛡️ Come funziona concretamente?

### Sequenza completa per un ingresso riuscito

1.  Inserimento del codice "4596" tramite telecomando IR.
2.  Arduino → `POST /api/access` con `{ "event": "door_open", "code": "4596", ... }` → API.
3.  L'API verifica il codice, vede che è valido, registra l'evento come "successo" e risponde.
4.  Arduino ← `{"event_status": "success"}` ← API.
5.  Arduino attiva il motore e visualizza "Apertura...".

### Sequenza per un'uscita

1.  L'utente preme il pulsante interno.
2.  Arduino attiva il motore per aprire la porta.
3.  Arduino → `POST /api/access` con `{ "event": "door_close", "code": "_LBE_", ... }` → API.

## 🌐 In sintesi

- **`POST`** = L'Arduino invia un **rapporto di azione** (che funge anche da domanda).
- **JSON** = Il formato dei rapporti e delle risposte.
- **WiFi** = Il mezzo di trasporto della comunicazione.

L'Arduino è l'agente sul campo che esegue e riferisce, l'API è il centro di controllo che autorizza e registra.