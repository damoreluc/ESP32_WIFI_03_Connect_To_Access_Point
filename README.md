# WiFi Esempio 03: connessione ad un Access Point e gestione riconnessione automatica

L'esempio illustra l'insieme delle operazioni necessarie per collegare la ESP32 ad un Access Point WiFi
e gestire le situazioni di perdita di connessione e riconnessione automatica.

Per il controllo dello stato della connessione vengono adoperati gli eventi dell'oggetto Wifi.

_Il progetto può essere impiegato come base per altri progetti che richiedono la connessione alla rete._

__NOTA__: le credenziali di accesso all'access point sono contenute nel file `WIFI/credentials.h`

In particolare, personalizzare con le proprie credenziali i parametri:

* `WIFI_SSID`
* `WIFI_PASSWORD`
