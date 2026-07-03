/**
 * @example Utilizzo dei parametri MQTT dal captive portal
 * 
 * Questo file mostra come utilizzare i parametri MQTT salvati
 * tramite il captive portal di WiFiManager
 */

#include <Arduino.h>
#include <WIFI/wifi_manager.h>

// Esempio di utilizzo in main.cpp:

/*
void setup()
{
  Serial.begin(115200);
  
  // Inizializza WiFiManager
  initWiFiManager("ESP32-WiFi", 180);
}

void loop()
{
  // Gestisci il portale WiFiManager
  handleWiFiManagerLoop();
  
  // Accedi ai parametri MQTT salvati
  Serial.print("MQTT Broker: ");
  Serial.println(getMQTTBroker());
  
  Serial.print("MQTT Client Name: ");
  Serial.println(getMQTTClientName());
  
  // Nota: getMQTTPassword() restituisce la password (usala con cautela)
  // Serial.print("MQTT Password: ");
  // Serial.println(getMQTTPassword());
  
  delay(5000);
}
*/

/**
 * COME FUNZIONA:
 * 
 * 1. Al primo avvio, WiFiManager crea un Access Point "ESP32-WiFi"
 * 
 * 2. Collegati all'AP e accedi a http://192.168.4.1
 * 
 * 3. Nel captive portal troverai i seguenti campi:
 *    - SSID WiFi
 *    - Password WiFi
 *    - MQTT Broker (es: "mqtt.example.com")
 *    - MQTT Password (password per il broker MQTT)
 *    - MQTT Client Name (nome del client MQTT)
 * 
 * 4. Compila tutti i campi e clicca "save"
 * 
 * 5. I parametri vengono salvati automaticamente in flash memory
 * 
 * 6. Nel tuo codice, recupera i parametri con:
 *    - getMQTTBroker()         -> ritorna l'indirizzo del broker
 *    - getMQTTPassword()       -> ritorna la password
 *    - getMQTTClientName()     -> ritorna il nome del client
 */

/**
 * VALORI DI DEFAULT:
 * 
 * Broker: "mqtt.example.com"
 * Password: "password"
 * Client Name: "ESP32_Client"
 * 
 * Puoi cambiarli nel captive portal in qualsiasi momento
 */

/**
 * RESET DELLA CONFIGURAZIONE:
 * 
 * Per resettare tutto (WiFi + parametri MQTT):
 * 
 *   resetWiFiConfig();   // Cancella tutto e riavvia
 * 
 * Il dispositivo tornerà in modalità configurazione al riavvio.
 */

/**
 * LIMITE LUNGHEZZA PARAMETRI:
 * 
 * - Broker MQTT: max 64 caratteri
 * - Password MQTT: max 32 caratteri
 * - Client Name: max 32 caratteri
 * 
 * Adatta i valori nel codice di wifi_manager.cpp se serve più spazio.
 */
