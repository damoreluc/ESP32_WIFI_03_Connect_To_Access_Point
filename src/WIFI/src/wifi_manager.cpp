#include <WIFI/wifi_manager.h>
#include <WIFI/wifi_functions.h>
#include <HWCONFIG/hwConfig.h>

// Istanza globale di WiFiManager (ESP_WiFiManager per ESP32)
WiFiManager wm;

// Variabili di stato
static bool wm_connected = false;
static uint32_t wm_timeout = 180000; // 180 sec di default

// Parametri MQTT personalizzati
//static char mqtt_broker[32] = "item2122a.cloud.shiftr.io";
static char mqtt_broker[32] = "mqtt.example.com";
static char mqtt_password[24] = "password";
static char mqtt_clientname[16] = "ESP32_Client";

// Oggetti WiFiManagerParameter per i parametri MQTT
WiFiManagerParameter custom_mqtt_broker("broker", "MQTT Broker", mqtt_broker, 32);
WiFiManagerParameter custom_mqtt_password("mqttpass", "MQTT Password", mqtt_password, 24);
WiFiManagerParameter custom_mqtt_clientname("clientname", "MQTT Client Name", mqtt_clientname, 16);

/**
 * @brief Callback chiamato quando WiFiManager salva i parametri
 */
void saveParamsCallback()
{
  Serial.println(F("[WiFiManager] Parametri salvati!"));
}

/**
 * @brief Callback chiamato quando WiFiManager entra in modalità AP
 */
void configModeCallback(WiFiManager *myWiFiManager)
{
  Serial.println(F("[WiFiManager] Modalità configurazione attiva"));
  Serial.print(F("[WiFiManager] SSID: "));
  Serial.println(myWiFiManager->getConfigPortalSSID());
  Serial.print(F("[WiFiManager] IP AP: "));
  Serial.println(WiFi.softAPIP());
  Serial.println(F("[WiFiManager] Accedi a http://192.168.4.1 per configurare il WiFi"));
  Serial.println(F("[WiFiManager] Portale web interno di WiFiManager attivo"));
  
  digitalWrite(pinWiFiConnected, LOW);
}

/**
 * @brief Callback chiamato quando la connessione WiFi ha successo
 */
void connectedCallback()
{
  Serial.println(F("[WiFiManager] WiFi collegato!"));
  wm_connected = true;
  
  // operazioni da compiere sui layers superiori dopo che il layer IP è pronto
  WiFiNetworkReady();
}

/**
 * @brief Callback chiamato quando la configurazione WiFiManager è salvata
 */
void configSavedCallback()
{
  Serial.println(F("[WiFiManager] Configurazione WiFi salvata nella flash memory"));
}

/**
 * @brief Handler per gli eventi WiFi
 */
void WiFiManagerEvent(WiFiEvent_t event)
{
  switch (event)
  {
  case ARDUINO_EVENT_WIFI_STA_CONNECTED:
    digitalWrite(pinWiFiConnected, HIGH);
    Serial.print(F("[WiFi] Connesso all'AP: "));
    Serial.print(WiFi.SSID());
    Serial.print(F(" (RSSI: "));
    Serial.print(WiFi.RSSI());
    Serial.println(")");
    break;

  case ARDUINO_EVENT_WIFI_STA_DISCONNECTED:
    digitalWrite(pinWiFiConnected, LOW);
    Serial.println(F("[WiFi] Disconnesso dall'Access Point"));
    wm_connected = false;
    WiFiNetworkFail();
    break;

  case ARDUINO_EVENT_WIFI_STA_GOT_IP:
    Serial.print(F("[WiFi] IP ottenuto: "));
    Serial.println(WiFi.localIP());
    wm_connected = true;
    connectedCallback();
    WiFiNetworkReady();
    break;

  case ARDUINO_EVENT_WIFI_STA_LOST_IP:
    Serial.println(F("[WiFi] IP perso"));
    wm_connected = false;
    WiFiNetworkFail();
    break;

  default:
    break;
  }
}

void initWiFiManager(const char* deviceName, uint32_t timeoutSeconds)
{
  Serial.println(F("\n[WiFiManager] Inizializzazione WiFiManager per ESP32..."));
  Serial.println(F("[WiFiManager] Con supporto captive portal e DNS server"));

  // Imposta il GPIO per il LED di connessione
  pinMode(pinWiFiConnected, OUTPUT);
  digitalWrite(pinWiFiConnected, LOW);

  // Salva il timeout
  wm_timeout = timeoutSeconds * 1000;

  // Configura WiFiManager per ESP32
  wm.setConfigPortalBlocking(false);     // Non blocca il resto del codice
  wm.setConfigPortalTimeout(timeoutSeconds);  // Timeout per il portale di configurazione
  wm.setConnectTimeout(20);              // Timeout per connessione: 20 sec
  wm.setAPCallback(configModeCallback);
  wm.setSaveConfigCallback(configSavedCallback);
  wm.setSaveParamsCallback(saveParamsCallback);
  
  // Abilita il debug sulla seriale
  wm.setDebugOutput(false);
  
  // Abilita il salvamento automatico di SSID e password
  wm.setBreakAfterConfig(true);

  // Aggiungi i parametri MQTT personalizzati al portale
  wm.addParameter(&custom_mqtt_broker);
  wm.addParameter(&custom_mqtt_password);
  wm.addParameter(&custom_mqtt_clientname);

  // Registra gli handler degli eventi WiFi
  WiFi.onEvent(WiFiManagerEvent, WiFiEvent_t::ARDUINO_EVENT_WIFI_STA_CONNECTED);
  WiFi.onEvent(WiFiManagerEvent, WiFiEvent_t::ARDUINO_EVENT_WIFI_STA_DISCONNECTED);
  WiFi.onEvent(WiFiManagerEvent, WiFiEvent_t::ARDUINO_EVENT_WIFI_STA_GOT_IP);
  WiFi.onEvent(WiFiManagerEvent, WiFiEvent_t::ARDUINO_EVENT_WIFI_STA_LOST_IP);

  // Prova a connettersi con le credenziali salvate
  // Se non trova credenziali valide, avvia il portale di configurazione
  bool res = wm.autoConnect(deviceName, "12345678");  // SSID e password dell'AP temporaneo

  if (res)
  {
    Serial.println(F("[WiFiManager] Connessione stabilita con successo!"));
    wm_connected = true;
    Serial.print(F("[WiFiManager] IP: "));
    Serial.println(WiFi.localIP());
    Serial.print(F("[WiFiManager] SSID: "));
    Serial.println(WiFi.SSID());
    
    // Leggi i parametri MQTT personalizzati
    strncpy(mqtt_broker, custom_mqtt_broker.getValue(), sizeof(mqtt_broker) - 1);
    strncpy(mqtt_password, custom_mqtt_password.getValue(), sizeof(mqtt_password) - 1);
    strncpy(mqtt_clientname, custom_mqtt_clientname.getValue(), sizeof(mqtt_clientname) - 1);
    
    Serial.println(F("[WiFiManager] Parametri MQTT letti:"));
    Serial.print(F("  - Broker: "));
    Serial.println(mqtt_broker);
    Serial.print(F("  - Client Name: "));
    Serial.println(mqtt_clientname);
    
    digitalWrite(pinWiFiConnected, HIGH);
    WiFiNetworkReady();
  }
  else
  {
    Serial.println(F("[WiFiManager] Timeout - Connessione non stabilita"));
    Serial.println(F("[WiFiManager] Modalità Access Point attiva"));
    Serial.println(String(F("[WiFiManager] SSID: ")) + deviceName);
    Serial.println(F("[WiFiManager] Password: 12345678"));
    Serial.println(F("[WiFiManager] IP AP: 192.168.4.1"));
    Serial.println(F("[WiFiManager] Accedi a http://192.168.4.1 per configurare il WiFi"));
    Serial.println(F("[WiFiManager] Oppure accedi a qualsiasi dominio (es: http://example.com)"));
    Serial.println(String(F("[WiFiManager] Portale rimane attivo per ")) + timeoutSeconds + F(" secondi"));
  }
}

void resetWiFiConfig()
{
  Serial.println(F("[WiFiManager] Resettaggio configurazione WiFi..."));
  
  // Rimuove le credenziali salvate
  wm.resetSettings();
  
  // Disconnette da WiFi
  WiFi.disconnect(true, true);  // turnOffWiFi, turnOffWiFiAP
  
  Serial.println(F("[WiFiManager] Configurazione resettata. Riavvio..."));
  delay(2000);
  ESP.restart();
}

/**
 * @brief Funzione da chiamare nel loop() per mantenere attivo WiFiManager
 */
void handleWiFiManagerLoop()
{
  // Elabora le richieste del portale WiFiManager
  wm.process();
}

/**
 * @brief Restituisce l'indirizzo del broker MQTT salvato
 */
const char* getMQTTBroker()
{
  return mqtt_broker;
}

/**
 * @brief Restituisce la password MQTT salvata
 */
const char* getMQTTPassword()
{
  return mqtt_password;
}

/**
 * @brief Restituisce il nome del client MQTT salvato
 */
const char* getMQTTClientName()
{
  return mqtt_clientname;
}
