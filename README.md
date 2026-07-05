# WiFi Esempio 03: connessione ad un Access Point e gestione riconnessione automatica

L'esempio illustra l'insieme delle operazioni necessarie per collegare la ESP32 ad un Access Point WiFi
e gestire le situazioni di perdita di connessione e riconnessione automatica.

Per il controllo dello stato della connessione vengono adoperati gli eventi dell'oggetto Wifi.

_Il progetto può essere impiegato come base per altri progetti che richiedono la connessione alla rete._

__NOTA__: le credenziali di accesso all'access point sono contenute nel file `WIFI/credentials.h`

In particolare, personalizzare con le proprie credenziali i parametri:

* `WIFI_SSID`
* `WIFI_PASSWORD`

## Modifiche Recenti

### Affinita core ESP32

La configurazione PlatformIO imposta in compilazione:

* `ARDUINO_RUNNING_CORE=1`
* `ARDUINO_EVENT_RUNNING_CORE=0`

Con questa configurazione:

* il task utente Arduino (`setup()` e `loop()`) gira su core 1
* lo stack eventi di rete Arduino gira su core 0

### Gestione connessione robusta

In inizializzazione WiFi STA:

* timeout massimo di connessione impostato a 20 secondi
* attesa non bloccante tramite `vTaskDelay(pdMS_TO_TICKS(1000))`
* se la connessione non riesce entro timeout, viene eseguito restart

### Registrazione eventi WiFi pulita

Gli handler ora registrano solo gli eventi necessari:

* `ARDUINO_EVENT_WIFI_STA_START`
* `ARDUINO_EVENT_WIFI_STA_CONNECTED`
* `ARDUINO_EVENT_WIFI_STA_DISCONNECTED`
* `ARDUINO_EVENT_WIFI_STA_GOT_IP`
* `ARDUINO_EVENT_WIFI_STA_LOST_IP`

Non viene piu usato `ARDUINO_EVENT_MAX`.

### Un solo handler eventi

La logica eventi e stata consolidata in un singolo handler `WiFiEvent(...)`:

* gestione connessione/disconnessione
* log reason code su disconnessione
* tentativo di reconnect
* callback verso `WiFiNetworkReady()` e `WiFiNetworkFail()`

I vecchi file handler separati sono stati rimossi per evitare duplicazioni.

### Diagnostica runtime

Sono state aggiunte stampe seriali di diagnostica per verificare:

* core corrente in `setup()`
* core corrente periodico in `loop()`
* core corrente in `initWiFi_STA()`
* core e id evento nelle callback WiFi

## Build Rapida

Compilazione release:

```bash
pio run -e release
```
