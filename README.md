# WiFi Esempio 03: connessione ad un Access Point con ESP32 WiFiManager Stack su Core 0

Questo progetto ESP32 usa WiFiManager in modalita bloccante per il provisioning iniziale della rete WiFi.
L'inizializzazione dello stack rete viene eseguita su Core 0 tramite task dedicato, mentre il loop applicativo
rimane su Core 1 senza chiamate dirette alle API WiFi.

## Funzioni introdotte per la gestione parametri WiFi con WiFiManager

Le funzioni seguenti sono esposte in [src/WIFI/wifi_manager.h](src/WIFI/wifi_manager.h) e implementate in [src/WIFI/src/wifi_manager.cpp](src/WIFI/src/wifi_manager.cpp).

1. initWiFiManager(const char* deviceName = "ESP32-WiFi", uint32_t timeoutSeconds = 180)
Gestisce tutto il flusso di provisioning:
- avvia captive portal se non ci sono credenziali valide
- configura timeout portal e timeout connessione
- registra callback WiFiManager e callback eventi WiFi
- legge i parametri custom MQTT dal portale
- rilascia memoria WiFiManager a fine provisioning (stop portal + delete istanza)

2. resetWiFiConfig()
Cancella configurazione WiFi salvata e riavvia il dispositivo.

3. getMQTTBroker()
Restituisce il valore del parametro custom MQTT Broker.

4. getMQTTPassword()
Restituisce il valore del parametro custom MQTT Password.

5. getMQTTClientName()
Restituisce il valore del parametro custom MQTT Client Name.

6. WiFiManagerEvent(WiFiEvent_t event)
Handler centralizzato degli eventi STA:
- connected
- disconnected
- got IP
- lost IP

### Callback interne usate per il provisioning

- saveParamsCallback(): conferma salvataggio parametri custom
- configModeCallback(WiFiManager*): log della modalita AP/captive portal
- configSavedCallback(): conferma salvataggio config WiFi
- connectedCallback(): notifica passaggio a network ready

## Elenco task in uso

### Task applicativi del progetto

1. WiFiManagerTask
- Sorgente: [src/WIFI/src/wifi_manager_task.cpp](src/WIFI/src/wifi_manager_task.cpp)
- Core: 0
- Priorita: 1
- Stack allocato: 20480 byte
- Ruolo: esegue initWiFiManager() e completa il provisioning rete
- Ciclo di vita: temporaneo, termina con vTaskDelete(NULL) a init completata

2. loopTask (Arduino)
- Sorgente logica: [src/main.cpp](src/main.cpp)
- Core: 1 (default Arduino su ESP32)
- Ruolo: logica applicativa periodica e aggiornamento LED stato WiFi

### Task di framework/sistema (FreeRTOS + Arduino/IDF)

- Event task Arduino WiFi su Core 0 (forzato da build flag ARDUINO_EVENT_RUNNING_CORE=0)
- Task di sistema FreeRTOS/IDF (idle, timer e networking interno)

Riferimento configurazione core eventi:
[platformio.ini](platformio.ini)

## Risorse di memoria disponibili al Core 1

Su ESP32 la RAM dinamica (heap) e condivisa tra i core; non esiste un heap separato esclusivo Core 1.
Le risorse effettivamente disponibili a Core 1 dipendono quindi dal carico totale di sistema.

### Valori utili del progetto (build release corrente)

- RAM totale target: 327680 byte
- RAM usata a build: 45492 byte
- RAM residua teorica globale: circa 282188 byte

Nota: il valore residuo reale a runtime varia in base a WiFi, stack rete e allocazioni dinamiche.

### Impatto delle scelte attuali su Core 1

- Lo stack rete applicativo viene avviato su Core 0, riducendo il carico diretto su Core 1
- WiFiManagerTask usa 20 KB su Core 0 ed e temporaneo
- A provisioning completato, l'istanza WiFiManager viene distrutta per recuperare heap

### Come misurare la memoria realmente disponibile per Core 1 a runtime

1. Heap libero globale dopo init:
- ESP.getFreeHeap()

2. Stack libero del task corrente su Core 1 (loopTask):
- uxTaskGetStackHighWaterMark(NULL)

3. Log gia presente nel progetto:
- stampa heap prima/dopo rilascio WiFiManager in [src/WIFI/src/wifi_manager.cpp](src/WIFI/src/wifi_manager.cpp)

## Flusso operativo sintetico

1. setup() avvia WiFiManagerTask su Core 0
2. WiFiManagerTask esegue initWiFiManager() in modalita bloccante
3. Se necessario, utente configura WiFi e parametri MQTT da captive portal
4. Parametri vengono salvati e rete passa a stato connesso
5. WiFiManager rilascia risorse memoria e task termina
6. Core 1 continua solo con loop applicativo
