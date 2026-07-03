# WiFiManager - Gestione Parametri WiFi (ESP_WiFiManager per ESP32)

## Panoramica

Questo progetto è stato aggiornato per utilizzare **ESP_WiFiManager**, la versione ottimizzata di WiFiManager specificatamente progettata per ESP32.

ESP_WiFiManager offre i seguenti vantaggi:

- ✅ **Compatibilità nativa con ESP32**
- ✅ **Nessun hardcoding** delle credenziali WiFi
- ✅ **Portale Web** per configurare il WiFi da qualsiasi dispositivo
- ✅ **Salvataggio automatico** delle credenziali in flash memory
- ✅ **Riconnessione automatica** con credenziali salvate
- ✅ **Gestione semplificata** degli stati di connessione

## Come Funziona

### Primo Avvio (First Boot)

1. La ESP32 si avvia e cerca le credenziali WiFi salvate in flash memory
2. Se non le trova (primo avvio), crea automaticamente un **Access Point temporaneo** con:
   - **SSID**: `ESP32-WiFi`
   - **Password**: `12345678`
3. Collegati a questo AP con il tuo smartphone/computer
4. Accedi a `http://192.168.4.1` nel browser
5. Seleziona la rete WiFi desiderata e inserisci la password
6. Le credenziali vengono salvate e la ESP32 si connette

### Avvii Successivi

La ESP32 si connette automaticamente alla rete WiFi salvata senza richiedere interventi.

Se la connessione fallisce, tenta il riavvio automatico dopo 20 secondi.

## File Aggiunti/Modificati

### File Nuovi

1. **`src/WIFI/wifi_manager.h`**
   - Header con le funzioni di gestione WiFiManager
   - Funzioni principali: `initWiFiManager()`, `resetWiFiConfig()`

2. **`src/WIFI/src/wifi_manager.cpp`**
   - Implementazione di WiFiManager
   - Gestione degli eventi WiFi
   - Callback per vari stati di connessione

### File Modificati

1. **`platformio.ini`**
   - Aggiunta dipendenza: `ESP_WiFiManager @ ^1.12.2` (versione per ESP32)

2. **`src/main.cpp`**
   - Aggiunto include `#include <WIFI/wifi_manager.h>`
   - Funzione `setup()` modificata per usare `initWiFiManager()` al posto di `initWiFi_STA()`

## Utilizzo

### Inizializzazione Semplice

```cpp
// Nel setup(), inizializza WiFiManager
initWiFiManager("ESP32-WiFi", 180);  // nome dispositivo, timeout secondi
```

### Resettare la Configurazione WiFi

Se desideri riconfigurare il WiFi da zero:

```cpp
resetWiFiConfig();  // Cancella credenziali e riavvia
```

## Output Seriale di Esempio

```
[WiFiManager] Inizializzazione WiFiManager per ESP32...
[WiFiManager] Connessione stabilita con successo!
[WiFiManager] IP: 192.168.1.100
[WiFiManager] SSID: MiaRete
[WiFi] Connesso all'AP: MiaRete (RSSI: -65)
[WiFi] IP ottenuto: 192.168.1.100
```
[WiFi] Connesso all'AP: MiaRete (RSSI: -65)
[WiFi] IP ottenuto: 192.168.1.100
```

### Se il WiFi non è Configurato

```
[WiFiManager] Inizializzazione WiFiManager...
[WiFiManager] Nessuna credenziale salvata
[WiFiManager] Modalità configurazione attiva
[WiFiManager] SSID: ESP32-WiFi
[WiFiManager] IP: 192.168.4.1
[WiFiManager] Accedi a http://192.168.4.1 per configurare il WiFi
```

## Spia LED

Il comportamento del LED di stato WiFi (connesso a `pinWiFiConnected`):

- 🔴 **SPENTO**: Dispositivo in configurazione o disconnesso
- 🟢 **ACCESO**: Dispositivo connesso a WiFi e ha un IP valido

## Timeout di Configurazione

Il portale di configurazione rimane aperto per **180 secondi** (3 minuti) dal primo avvio.

Se vuoi cambiare questo timeout:

```cpp
initWiFiManager("ESP32-WiFi", 300);  // 5 minuti
```

## Modifica della Password dell'AP Temporaneo

Per cambiare la password dell'Access Point temporaneo, modifica in `wifi_manager.cpp`:

```cpp
bool res = wm.autoConnect("ESP32-WiFi", "NUOVA_PASSWORD");
```

## Disabilitazione della Modalità Portale

Se desideri che il dispositivo non entri in modalità configurazione:

```cpp
wm.setConfigPortalBlocking(true);  // Blocca finché non è configurato
```

## Risoluzione dei Problemi

### Il Portale Non Appare

1. Verifica che la ESP32 sia accesa
2. Cerca nelle reti disponibili un AP con nome "ESP32-WiFi"
3. Controlla i messaggi seriali sulla porta UART

### Il Dispositivo Non si Connette

1. Verifica che le credenziali WiFi siano corrette nel portale
2. Controlla la distanza dal router
3. Verifica che il router non abbia restrizioni MAC

### Vuoi Cancellare le Credenziali

Chiama `resetWiFiConfig()` per resettare tutto e riconfigurare da zero.

## Prossimi Passi

Puoi aggiungere ulteriori parametri personalizzati al portale di WiFiManager:

```cpp
// Aggiungi parametri custom al portale
WiFiManagerParameter custom_param("key", "label", "default", 20);
wm.addParameter(&custom_param);
```

Per ulteriori informazioni, consulta la [documentazione ufficiale di WiFiManager](https://github.com/tzapu/WiFiManager).
