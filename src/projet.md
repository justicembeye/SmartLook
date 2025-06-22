
-----

# 🔐 Progetto: **SmartLock — Serratura Intelligente e Sicura per Siti Tecnici Isolati**

-----

## 🎯 Obiettivo del Progetto

> Progettare un sistema di chiusura intelligente e connesso, che combina **hardware pilotato da un microcontrollore** e **API web**.
> Il sistema deve permettere:

- Un inserimento di codice tramite telecomando IR.
- Una chiara visualizzazione dello stato su un display LCD.
- Uno sblocco tramite un motore comandato.
- Una registrazione centralizzata degli accessi e delle uscite tramite un'API.
- Una gestione robusta degli errori e degli allarmi di sicurezza.
- Un'uscita manuale semplice e sicura tramite un pulsante fisico.

-----

## 🧩 Funzionalità Principali

| Funzionalità                   | Descrizione                                                                                                                                                                |
|--------------------------------|----------------------------------------------------------------------------------------------------------------------------------------------------------------------------|
| 🔐 **Inserimento codice IR**   | Inserimento di un codice a 4 cifre tramite un telecomando a infrarossi standard.                                                                                           |
| 🧠 **Logica Embedded**         | Il microcontrollore **Arduino Nano ESP32** gestisce tutta la logica: decodifica IR, macchina a stati per le operazioni, comunicazione WiFi e pilotaggio delle periferiche. |
| 🖥️ **Display LCD**            | Un display LCD I2C 16x2 visualizza lo stato del sistema, il codice in fase di inserimento, i messaggi di successo o errore, e gli allarmi.                                 |
| 📡 **Connessione API**         | L'ESP32 si connette in WiFi per comunicare con un'API Flask, che funge da autorità centrale per la validazione e la registrazione.                                         |
| 🔓 **Sblocco**                 | Comando di un motore tramite un driver L293D per l'azionamento della serratura.                                                                                            |
| 🔘 **Pulsante interno**        | Permette di attivare un'uscita senza codice. Il sistema distingue un'uscita "normale" da un'uscita "sospetta".                                                             |
| 🧲 **Rilevamento fisico**      | Due micro-switch rilevano con precisione se la porta è in posizione "aperta" o "chiusa".                                                                                   |
| 📜 **Registrazione (Logging)** | Ogni evento (tentativo, apertura, chiusura, allarme) viene marcato temporalmente e inviato all'API per essere registrato.                                                  |
| 🚨 **Sicurezza**               | Un allarme viene inviato dopo 3 errori di codice, il sistema si blocca temporaneamente e viene rilevata l'apertura forzata.                                                |

-----

## 📌 Casi d'Uso (Scenari Reali)

### 🎮 1. **Ingresso tramite telecomando IR**

- L'operatore punta il ricevitore IR e digita 4 cifre sul telecomando.
- Il modulo `ir_receiver_manager` dell'Arduino decodifica ogni segnale IR e lo mappa alla cifra corrispondente (es: `0xFF18E7ULL` -\> `'2'`).
- Il modulo `ir_code_input_logic` assembla le cifre una per una e le visualizza sul LCD tramite il `display_manager`.

### 🔐 2. **Validazione del codice**

- Una volta inserite le 4 cifre, `ir_code_input_logic` passa il codice completo al `door_operation_manager` (il cervello).
- Il `door_operation_manager` entra nello stato `PENDING_API_VALIDATION` e chiede ad `api_client` di inviare una richiesta `POST /api/access` con l'evento `door_open` e il codice inserito.
- **Se l'API risponde "successo":**
    - Il `feedback_manager` emette un segnale di successo (LED verde + suono).
    - Il `door_operation_manager` comanda al `door_motor_manager` di aprire la serratura.
    - Il LCD visualizza "Apertura...".
- **Se l'API risponde "fallimento":**
    - Il LCD visualizza "Codice Invalido" o "Codice Scaduto".
    - Il `feedback_manager` emette un segnale di errore (LED rosso + suono).
    - Dopo 3 errori, un allarme viene inviato all'API e il sistema entra nello stato `LOCKED_OUT`.

### 🚪 3. **Gestione della porta (Apertura e Chiusura)**

- Quando la porta è fisicamente aperta, il sensore corrispondente viene attivato. Il `door_operation_manager` lo rileva tramite il `door_sensor_manager` e passa allo stato `IDLE_OPEN`, avviando un timer di auto-chiusura.
- Alla scadenza del timer, il `door_operation_manager` comanda la chiusura della porta.

### 🔘 4. **Uscita tramite pulsante interno**

- L'operatore preme il pulsante di uscita.
- `input_manager` rileva la pressione e la segnala al `door_operation_manager`.
- Il `door_operation_manager` comanda l'apertura e invia una richiesta `POST /api/access` con l'evento `door_close` per registrare l'uscita.

-----

## 🔧 Hardware Effettivamente Utilizzato

| Componente                   | Ruolo                                                                     |
|------------------------------|---------------------------------------------------------------------------|
| **Arduino Nano ESP32**       | Elaborazione centrale, logica della macchina a stati e connettività WiFi. |
| Telecomando IR + Ricevitore  | Input utente per il codice di accesso.                                    |
| Display LCD 16x2 I2C         | Feedback visivo per l'utente.                                             |
| Motore DC + Driver L293D     | Attuatore della serratura.                                                |
| 2 Micro-switch               | Rilevamento preciso delle posizioni "porta aperta" e "porta chiusa".      |
| Pulsante                     | Attivatore manuale della procedura di uscita.                             |
| LED RGB + Buzzer             | Segnalazione di stato (successo, errore, allarme).                        |
| Breadboard, resistenze, cavi | Assemblaggio generale del circuito.                                       |

-----

## 🧠 Architettura di Sistema

```mermaid
flowchart TD
    subgraph "Dispositivo Hardware"
        IR[📡 Ricevitore IR] --> MCU
        SENSORS[🧲 Sensori Porta] --> MCU
        BUTTON[🔘 Pulsante Uscita] --> MCU

        MCU[🧠 Arduino Nano ESP32 <br><i>(Macchina a Stati)</i>]

        MCU --> MOTOR[⚙️ Driver Motore L293D]
        MCU -- I2C --> LCD[📺 Display LCD]
        MCU --> FEEDBACK[🔊 LED/Buzzer]
    end

    subgraph "Server Centrale"
        API[🌐 API Python Flask]
        DB[(💾 codes.json)]
        API <--> DB
    end

    MCU <-- WiFi / HTTP --> API
```

-----

## 🌐 Lato API Flask

| Endpoint        | Metodo | Utilità nel Progetto                                                                                                     |
|-----------------|--------|--------------------------------------------------------------------------------------------------------------------------|
| `/api/access`   | `POST` | **Punto centrale per l'Arduino.** Permette di validare un codice (`door_open`) e di registrare un'uscita (`door_close`). |
| `/api/alert`    | `POST` | Permette all'Arduino di segnalare una situazione anomala (tentativi falliti, forzatura).                                 |
| `/api/settings` | `GET`  | Permette all'Arduino di recuperare la sua configurazione all'avvio (es: max errori).                                     |
| `/` (Dashboard) | `GET`  | Fornisce un'interfaccia web di supervisione per visualizzare i log e gli allarmi.                                        |
| `/api/code`     | `POST` | Utilizzato dalla dashboard per generare un nuovo codice monouso.                                                         |

-----

## 📋 Fasi di Realizzazione (Bilancio)

### Fase 1 – API & Server (Completata)

- [x] Definire la struttura dell'API con Flask e Flask-RESTful.
- [x] Creare gli endpoint `/api/access`, `/api/alert`, `/api/settings` e `/api/code`.
- [x] Implementare la logica di generazione del codice, di registrazione e di gestione degli allarmi in `codes.json`.
- [x] Creare un'interfaccia dashboard semplice in HTML/Jinja2.
- [x] Validare il funzionamento dell'API con script di test `test_api.py`.

### Fase 2 – Microcontrollore & Logica Embedded (Completata)

- [x] Leggere i tasti IR e mapparli a caratteri utilizzando la libreria `IRremoteESP8266`.
- [x] Gestire l'inserimento e la visualizzazione del codice in modo interattivo sul LCD.
- [x] Sviluppare un client HTTP robusto (`api_client`) per comunicare con il server Flask.
- [x] Implementare una macchina a stati finiti (`door_operation_manager`) per orchestrare tutte le operazioni in modo affidabile.
- [x] Pilotare il motore, i sensori, il pulsante e i moduli di feedback, ciascuno nel proprio modulo.
- [x] Implementare uno scheduler di task (`scheduler`) per un funzionamento non bloccante.

-----