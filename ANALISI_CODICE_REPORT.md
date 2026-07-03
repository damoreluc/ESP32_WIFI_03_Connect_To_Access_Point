# Analisi Codice - ESP32 WiFi Connection Project
## Report di Analisi e Correzioni

**Data:** 10 Giugno 2026  
**Progetto:** ESP32_WIFI_03_Connect_To_Access_Point  
**Piattaforma:** ESP32 (WROOM-32D)

---

## 📋 Sommario Esecutivo

Sono stati identificati **11 punti critici** nel codice. La maggior parte riguarda:
- **Blocco del sistema** durante la connessione WiFi (setup)
- **Mancanza di timeout** e gestione di errori
- **Incoerenza nella registrazione** degli event handler
- **Problemi di sicurezza** (credenziali hardcoded)
- **Logica duplicata** nella gestione degli eventi

---

## 🔴 Problemi Identificati

### 1. **CRITICO: Blocco infinito nel setup() - while loop senza timeout**

**File:** [src/WIFI/src/wifi_init_STA.cpp](src/WIFI/src/wifi_init_STA.cpp#L14-L18)

**Problema:**
```cpp
while (WiFi.status() != WL_CONNECTED) {
    Serial.print('.');
    delay(1000);
}
```

- Il loop blocca indefinitamente se le credenziali sono errate
- Se l'Access Point non è disponibile, il dispositivo rimane bloccato al boot
- Non è possibile eseguire altre operazioni
- `delay(1000)` è bloccante (non consente al task scheduler di funzionare)

**Impatto:** CRITICO - Il dispositivo diventa non responsivo

**Correzione Consigliata:**
```cpp
// Timeout: max 20 secondi per connessione
unsigned long startTime = millis();
const unsigned long timeout = 20000; // 20 sec
const unsigned int maxAttempts = 20;
unsigned int attempts = 0;

while (WiFi.status() != WL_CONNECTED && millis() - startTime < timeout) {
    Serial.print('.');
    vTaskDelay(pdMS_TO_TICKS(1000)); // Non-blocking delay
    attempts++;
}

if (WiFi.status() != WL_CONNECTED) {
    Serial.println("\n[ERROR] WiFi connection timeout!");
    // Implementare fallback: restart, AP mode, ecc.
} else {
    Serial.printf("\nConnected in %d attempts\n", attempts);
}
```

---

### 2. **CRITICO: Registrazione evento errata - ARDUINO_EVENT_MAX registra tutti gli eventi**

**File:** [src/WIFI/src/wifi_init_STA.cpp](src/WIFI/src/wifi_init_STA.cpp#L11)

**Problema:**
```cpp
WiFi.onEvent(WiFiEvent, WiFiEvent_t::ARDUINO_EVENT_MAX);
```

- `ARDUINO_EVENT_MAX` è un valore massimo, NON un tipo di evento valido
- Registra TUTTI gli eventi sulla funzione `WiFiEvent()`
- Le funzioni `WiFiStationConnected()` e `WiFiGotIP()` sono **commentate** e mai chiamate
- Sprechi di risorse elaborando tutti gli eventi quando ne servono solo alcuni

**Impatto:** ALTO - Logica confusa, funzioni mai utilizzate

**Correzione:**
```cpp
// Registrare solo gli eventi necessari
WiFi.onEvent(WiFiEvent, ARDUINO_EVENT_WIFI_STA_CONNECTED);
WiFi.onEvent(WiFiEvent, ARDUINO_EVENT_WIFI_STA_DISCONNECTED);
WiFi.onEvent(WiFiEvent, ARDUINO_EVENT_WIFI_STA_GOT_IP);
WiFi.onEvent(WiFiEvent, ARDUINO_EVENT_WIFI_STA_LOST_IP);

// Oppure usare il gestore generico per tutti gli eventi WiFi
WiFi.onEvent(WiFiEvent, ARDUINO_EVENT_WIFI_STA_START);
```

---

### 3. **ALTO: Funzione WiFiGotIP() non viene mai chiamata**

**File:** [src/WIFI/src/wifi_got_IP.cpp](src/WIFI/src/wifi_got_IP.cpp)

**Problema:**
- La funzione è definita ma mai registrata come event handler
- Nel `wifi_init_STA.cpp` la registrazione è commentata
- Codice morto: non serve a nulla nella configurazione attuale

**Impatto:** ALTO - Codice non utilizzato, confusione

**Soluzione:**
- Eliminare la funzione non utilizzata, OPPURE
- Registrare correttamente l'handler come descritto nel punto 2

---

### 4. **ALTO: Duplicazione logica nella gestione della connessione**

**File:** [src/WIFI/src/wifi_events.cpp](src/WIFI/src/wifi_events.cpp#L42-L48) vs [src/WIFI/src/wifi_station_connected.cpp](src/WIFI/src/wifi_station_connected.cpp)

**Problema:**
- `WiFiEvent()` gestisce `ARDUINO_EVENT_WIFI_STA_CONNECTED` e accende il LED
- `WiFiStationConnected()` fa la stessa cosa ma non è mai chiamata
- Logica duplicata e incoerente

**Impatto:** MEDIO - Codice confuso, difficile da manutenere

**Correzione:**
Eliminare `WiFiStationConnected()` e gestire tutto in `WiFiEvent()`

---

### 5. **ALTO: Mancanza di protezione dello stack WiFi - possibile race condition**

**File:** [src/WIFI/src/wifi_events.cpp](src/WIFI/src/wifi_events.cpp)

**Problema:**
- La funzione `WiFiEvent()` viene chiamata da un interrupt/task diverso
- Non è thread-safe l'accesso a `digitalWrite(pinWiFiConnected, ...)` senza sincronizzazione
- Potenziale race condition con il `loop()` principale

**Impatto:** MEDIO - Instabilità sporadica, difficile da debuggare

**Correzione:**
```cpp
// Usa una flag atomica per comunicare con il main loop
static volatile bool wifiConnected = false;

void WiFiEvent(WiFiEvent_t event) {
    switch(event) {
        case ARDUINO_EVENT_WIFI_STA_CONNECTED:
            wifiConnected = true;
            break;
        case ARDUINO_EVENT_WIFI_STA_DISCONNECTED:
            wifiConnected = false;
            break;
    }
}

void loop() {
    // Leggi la flag atomica in modo sicuro
    if (wifiConnected) {
        digitalWrite(pinWiFiConnected, HIGH);
    } else {
        digitalWrite(pinWiFiConnected, LOW);
    }
}
```

---

### 6. **MEDIO: Credenziali WiFi hardcoded nel file source**

**File:** [src/WIFI/credentials.h](src/WIFI/credentials.h)

**Problema:**
```cpp
#define WIFI_SSID "SOSTITUIRE_COL_TUO_SSID"
#define WIFI_PASSWORD "SOSTITUIRE_CON_TUA_PASSWORD"
```

- Credenziali in plain text nel sorgente
- Non è possibile cambiarle senza ricompilare
- Se il file viene commitato in Git, le credenziali sarebbero visibili nella storia
- Problema di sicurezza in ambiente di produzione

**Impatto:** MEDIO - Sicurezza e manutenibilità

**Correzione:**
```cpp
// Memorizzare in NVS (Non-Volatile Storage) di ESP32

#include <nvs_flash.h>
#include <nvs.h>

void initWiFiCredentials() {
    nvs_handle_t nvsHandle;
    esp_err_t err = nvs_open("wifi", NVS_READWRITE, &nvsHandle);
    
    if (err == ESP_OK) {
        size_t ssidLen = 32;
        size_t passLen = 64;
        char ssid[32] = {0};
        char password[64] = {0};
        
        nvs_get_str(nvsHandle, "ssid", ssid, &ssidLen);
        nvs_get_str(nvsHandle, "pass", password, &passLen);
        
        if (strlen(ssid) > 0) {
            WiFi.begin(ssid, password);
        } else {
            // Usare credenziali di default
        }
        
        nvs_close(nvsHandle);
    }
}
```

---

### 7. **MEDIO: Mancanza di timeout nell'evento DISCONNECTED**

**File:** [src/WIFI/src/wifi_station_disconnected.cpp](src/WIFI/src/wifi_station_disconnected.cpp#L8-L10)

**Problema:**
```cpp
void WiFiStationDisconnected(WiFiEvent_t event, WiFiEventInfo_t info) {
    // ...
    WiFi.reconnect();
}
```

- `WiFi.reconnect()` viene chiamato immediatamente ad ogni disconnessione
- Se il router è spento, questo creerà un loop di riconnessioni rapide
- Consuma corrente inutilmente e può causare instabilità
- Manca backoff esponenziale

**Impatto:** MEDIO - Consumo energetico, instabilità

**Correzione:**
```cpp
static unsigned long lastReconnectTime = 0;
static unsigned int reconnectAttempts = 0;
const unsigned int maxReconnectAttempts = 5;
const unsigned long reconnectBackoffMs = 5000; // 5 sec

void WiFiStationDisconnected(WiFiEvent_t event, WiFiEventInfo_t info) {
    Serial.print(F("WiFi lost. Reason: "));
    Serial.println(info.wifi_sta_disconnected.reason);
    
    if (millis() - lastReconnectTime < reconnectBackoffMs) {
        Serial.println(F("Too soon for reconnection, waiting..."));
        return;
    }
    
    if (reconnectAttempts < maxReconnectAttempts) {
        Serial.println(F("Attempting to reconnect..."));
        WiFi.reconnect();
        lastReconnectTime = millis();
        reconnectAttempts++;
    } else {
        Serial.println(F("Max reconnection attempts reached. Restarting..."));
        ESP.restart();
    }
}
```

---

### 8. **MEDIO: Mancanza di validazione delle credenziali**

**File:** [src/WIFI/src/wifi_init_STA.cpp](src/WIFI/src/wifi_init_STA.cpp#L13)

**Problema:**
```cpp
WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
```

- Non viene verificato se le credenziali non sono nulle/vuote
- Se `WIFI_SSID` o `WIFI_PASSWORD` contengono i valori di default (placeholder), la connessione fallirà comunque
- Nessun messaggio di errore utile

**Impatto:** BASSO/MEDIO - UX scarsa

**Correzione:**
```cpp
// Validazione input
if (strlen(WIFI_SSID) == 0 || strcmp(WIFI_SSID, "SOSTITUIRE_COL_TUO_SSID") == 0) {
    Serial.println("[ERROR] WiFi SSID not configured! Please set WIFI_SSID");
    return false;
}

if (strlen(WIFI_PASSWORD) == 0) {
    Serial.println("[WARNING] WiFi Password is empty!");
}

WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
```

---

### 9. **MEDIO: Uso di Serial.print prima che Serial sia inizializzato correttamente**

**File:** [src/main.cpp](src/main.cpp#L18-L19)

**Problema:**
```cpp
void setup() {
    Serial.begin(115200);
    Serial.println("WiFi Esempio 03..."); // Potrebbe stampare garbage
```

- Dopo `Serial.begin()` ci potrebbe essere un breve delay prima che Serial sia pronto
- I primi caratteri potrebbero non essere stampati correttamente

**Impatto:** BASSO - Non critico su ESP32, ma buona pratica

**Correzione:**
```cpp
void setup() {
    Serial.begin(115200);
    delay(100); // Piccolo delay per stabilizzare
    
    // Se possibile, aspettare il collegamento seriale (per debug USB)
    while (!Serial && millis() < 5000) {
        delay(10);
    }
    
    Serial.println("\nWiFi Esempio 03: connessione ad un Access Point...");
}
```

---

### 10. **BASSO: LED non inizializzato se hwConfig.h non è incluso correttamente**

**File:** [src/HWCONFIG/hwConfig.h](src/HWCONFIG/hwConfig.h#L9)

**Problema:**
- `pinWiFiConnected` è definito su GPIO_NUM_23
- Non c'è verifica che il pin sia valido
- Se il pin è usato per altro scopo, potrebbe causare conflitti

**Impatto:** BASSO - Ma potenziale per problemi futuri

**Correzione:**
```cpp
#define pinWiFiConnected GPIO_NUM_23

// Validazione al compile time se possibile
#if (pinWiFiConnected < 0 || pinWiFiConnected > 39)
    #error "Invalid GPIO pin for WiFi LED"
#endif
```

---

### 11. **BASSO: Funzioni vuote in CUSTOM folder**

**File:** [src/WIFI/CUSTOM/wifi_network_ready.cpp](src/WIFI/CUSTOM/wifi_network_ready.cpp) e [src/WIFI/CUSTOM/wifi_network_fail.cpp](src/WIFI/CUSTOM/wifi_network_fail.cpp)

**Problema:**
```cpp
void WiFiNetworkReady() {
    // Vuota
}

void WiFiNetworkFail() {
    // Vuota
}
```

- Le funzioni sono chiamate ma non implementate
- Nessuna logica per gestire lo stato di network pronto/guasto
- Commento dice "Operazioni dei layer superiori" ma non c'è niente

**Impatto:** BASSO - Ma riduce utilità del codice

**Correzione:**
Implementare le funzioni con logica reale:
```cpp
void WiFiNetworkReady() {
    Serial.println("[EVENT] WiFi Network is READY - IP layer operational");
    digitalWrite(pinWiFiConnected, HIGH);
    // TODO: Sincronizzare orario con NTP
    // TODO: Connettere a cloud/server
}

void WiFiNetworkFail() {
    Serial.println("[EVENT] WiFi Network FAILED - IP layer down");
    digitalWrite(pinWiFiConnected, LOW);
    // TODO: Salvare dati localmente
    // TODO: Entrare in modalità offline
}
```

---

## 🛠️ Riepilogo Correzioni Consigliate

| Priorità | Problema | Correzione Rapida |
|----------|----------|-------------------|
| 🔴 CRITICA | Blocco setup | Aggiungere timeout e sostituire `delay()` con `vTaskDelay()` |
| 🔴 CRITICA | Evento MAX errato | Registrare solo gli eventi necessari |
| 🟠 ALTA | WiFiGotIP non utilizzata | Eliminare funzione o registrare handler |
| 🟠 ALTA | Duplicazione logica | Consolidare gestione eventi |
| 🟡 MEDIA | Race condition LED | Usare flag atomica |
| 🟡 MEDIA | Credenziali hardcoded | Migrare a NVS storage |
| 🟡 MEDIA | Reconnect senza backoff | Aggiungere backoff esponenziale |
| 🟡 MEDIA | Credenziali non validate | Aggiungere validazione input |
| 🔵 BASSA | Serial non stabile | Aggiungere piccolo delay |
| 🔵 BASSA | LED no GPIO check | Aggiungere validazione pin |
| 🔵 BASSA | Funzioni vuote | Implementare logica nelle custom functions |

---

## 📊 Metriche Qualità Codice

```
Problemi Trovati:        11
  - Critici:             2
  - Alti:                3
  - Medi:                4
  - Bassi:               2

Linee di codice:         ~250
Complexity:              Media
Sicurezza:               Bassa (credenziali)
Robustezza:              Bassa (no timeout/fallback)
Manutenibilità:          Media (duplicazione logica)
```

---

## ✅ Checklist Miglioramenti

- [ ] Aggiungere timeout nel setup WiFi
- [ ] Correggere registrazione eventi
- [ ] Eliminare funzioni non utilizzate
- [ ] Aggiungere backoff per reconnect
- [ ] Migrare credenziali a NVS
- [ ] Aggiungere validazione input
- [ ] Implementare WiFiNetworkReady/Fail
- [ ] Aggiungere thread-safety per LED
- [ ] Test timeout/edge cases
- [ ] Documentazione configurazione

---

## 📝 Note Aggiuntive

### Struttura Progetto
La struttura è buona, ben organizzata con separazione tra:
- Hardware config (HWCONFIG)
- WiFi functions (WIFI)
- Implementazioni custom (CUSTOM)

### Punti Positivi
✅ Uso corretto di events ESP32  
✅ Struttura modulare  
✅ Documentazione degli eventi  

### Aree di Miglioramento
⚠️ Gestione errori debole  
⚠️ Mancanza di timeout  
⚠️ Sicurezza credenziali  
⚠️ Thread-safety  

---

**Report generato automaticamente - Analisi Codice Completata**
