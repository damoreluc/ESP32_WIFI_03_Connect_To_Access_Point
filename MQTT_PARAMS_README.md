# Parametri MQTT nel Captive Portal di WiFiManager

## Panoramica

Il captive portal di WiFiManager è stato esteso per includere tre parametri personalizzati per MQTT:

1. **MQTT Broker** - Indirizzo del broker MQTT (es: `mqtt.example.com`)
2. **MQTT Password** - Password di accesso al broker MQTT
3. **MQTT Client Name** - Nome del client MQTT (es: `ESP32_Client`)

Questi parametri vengono salvati automaticamente in flash memory insieme alle credenziali WiFi.

## Come Usare

### 1. Accesso al Portale di Configurazione

**Primo Avvio:**
- La ESP32 crea un Access Point temporaneo: `ESP32-WiFi`
- Password dell'AP: `12345678`
- Accedi a `http://192.168.4.1` dal browser

**Nei successivi avvii:**
- Se la configurazione è già salvata, la ESP32 si connette automaticamente

### 2. Compilazione dei Parametri MQTT

Nel portale web troverai questi campi:

```
WiFi SSID:           [_______________]
WiFi Password:       [_______________]
MQTT Broker:         [mqtt.example.com_]
MQTT Password:       [password____]
MQTT Client Name:    [ESP32_Client_]
```

Compila tutti i campi e clicca **"save"**

### 3. Lettura dei Parametri nel Codice

Nel tuo sketch, accedi ai parametri usando queste funzioni:

```cpp
#include <WIFI/wifi_manager.h>

void setup()
{
  Serial.begin(115200);
  initWiFiManager("ESP32-WiFi", 180);
}

void loop()
{
  handleWiFiManagerLoop();
  
  // Leggi i parametri MQTT salvati
  const char* broker = getMQTTBroker();
  const char* password = getMQTTPassword();
  const char* clientName = getMQTTClientName();
  
  Serial.print("Broker: ");
  Serial.println(broker);
  Serial.print("Client: ");
  Serial.println(clientName);
  
  delay(5000);
}
```

## Funzioni Disponibili

### `getMQTTBroker()`
Restituisce l'indirizzo del broker MQTT salvato.

```cpp
const char* broker = getMQTTBroker();
// Ritorna: "mqtt.example.com"
```

### `getMQTTPassword()`
Restituisce la password MQTT salvata.

```cpp
const char* password = getMQTTPassword();
// Ritorna: "password"
```

### `getMQTTClientName()`
Restituisce il nome del client MQTT salvato.

```cpp
const char* clientName = getMQTTClientName();
// Ritorna: "ESP32_Client"
```

## Valori di Default

Se non configuri i parametri, verranno utilizzati questi valori:

```cpp
Broker:      "mqtt.example.com"
Password:    "password"
Client Name: "ESP32_Client"
```

## Limiti di Lunghezza

I parametri hanno i seguenti limiti:

| Parametro | Max Lunghezza |
|-----------|---------------|
| MQTT Broker | 64 caratteri |
| MQTT Password | 32 caratteri |
| MQTT Client Name | 32 caratteri |

Se hai bisogno di valori più lunghi, modifica in `wifi_manager.cpp`:

```cpp
static char mqtt_broker[128] = "mqtt.example.com";     // da 64 a 128
static char mqtt_password[64] = "password";             // da 32 a 64
static char mqtt_clientname[64] = "ESP32_Client";       // da 32 a 64
```

## Salvataggio in Flash Memory

I parametri vengono salvati automaticamente in flash memory quando clicchi "save" nel portale.

La configurazione persiste anche dopo lo spegnimento del dispositivo.

## Reset della Configurazione

### Ripristino Completo

Per cancellare tutto (WiFi + parametri MQTT) e riavviare il dispositivo:

```cpp
resetWiFiConfig();
```

Il dispositivo si riavvierà e tornerà in modalità configurazione.

### Reset Selettivo

Se vuoi cambiare solo i parametri MQTT (non il WiFi), riaccedi al portale di configurazione seguendo i passi sopra.

## Esempio Completo

```cpp
#include <Arduino.h>
#include <WiFi.h>
#include <WIFI/wifi_manager.h>

void setup()
{
  Serial.begin(115200);
  Serial.println("\n\nAvvio ESP32 con parametri MQTT");
  
  // Inizializza WiFiManager con parametri MQTT
  initWiFiManager("ESP32-WiFi", 180);
}

void loop()
{
  // Mantieni attivo il portale WiFiManager
  handleWiFiManagerLoop();
  
  // Dopo la connessione, accedi ai parametri
  if (WiFi.status() == WL_CONNECTED)
  {
    Serial.println("[INFO] WiFi Connesso");
    Serial.print("[MQTT] Broker: ");
    Serial.println(getMQTTBroker());
    Serial.print("[MQTT] Client: ");
    Serial.println(getMQTTClientName());
  }
  
  vTaskDelay(pdMS_TO_TICKS(5000));
}
```

## Output Seriale di Esempio

Quando la ESP32 si connette con i parametri salvati:

```
[WiFiManager] Inizializzazione WiFiManager per ESP32...
[WiFiManager] Connessione stabilita con successo!
[WiFiManager] IP: 192.168.1.100
[WiFiManager] SSID: MiaRete
[WiFiManager] Parametri MQTT letti:
  - Broker: mqtt.example.com
  - Client Name: ESP32_Client
[WiFi] Connesso all'AP: MiaRete (RSSI: -65)
[WiFi] IP ottenuto: 192.168.1.100
```

## Risoluzione Problemi

### I parametri non vengono visualizzati nel portale

1. Verifica di aver compilato e caricato il firmware aggiornato
2. Prova a cancellare la configurazione: `resetWiFiConfig()`
3. Riaccedi al portale dopo il riavvio

### I parametri non vengono salvati

1. Controlla il debug seriale per messaggi di errore
2. Verifica che i valori non superino i limiti di lunghezza
3. Riprova il salvataggio

### Come modificare i valori di default

Modifica in `wifi_manager.cpp`:

```cpp
static char mqtt_broker[64] = "nuovo.broker.com";
static char mqtt_password[32] = "nuova_password";
static char mqtt_clientname[32] = "NuovoNomeClient";
```

Quindi ricompila e carica il firmware.

## Note di Sicurezza

- La password MQTT viene salvata in flash memory in chiaro
- Se la sicurezza è critica, considera di criptare i dati MQTT
- Usa connessioni TLS/SSL quando possibile per il protocollo MQTT

## Supporto

Se hai domande o problemi, consulta la [documentazione di WiFiManager](https://github.com/tzapu/WiFiManager).
