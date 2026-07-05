/*
 * WiFi Esempio 03: connessione ad un Access Point e gestione riconnessione automatica
 *
 * L'esempio illustra l'insieme delle operazioni necessarie per collegare la ESP32 ad un Access Point WiFi
 * e gestire le situazioni di perdita di connessione e riconnessione automatica.
 *
 * Integrazione con WiFiManager: gestione automatica dei parametri WiFi tramite portale web
 * Se le credenziali non sono salvate o non valide, WiFiManager crea un Access Point temporaneo
 * su cui è possibile configurare il WiFi mediante browser.
 *
 * Per il controllo dello stato della connessione vengono adoperati gli eventi dell'oggetto Wifi
 *
 */

#include <Arduino.h>
#include <HWCONFIG/hwConfig.h>
#include <WiFi.h>
#include <WIFI/wifi_functions.h>
#include <WIFI/wifi_manager.h>
#include <WIFI/wifi_manager_task.h>

void setup()
{
  Serial.begin(115200);
  delay(100);
  Serial.println("\n\nWiFi Esempio 03: connessione ad un Access Point e gestione riconnessione automatica");
  Serial.println("Con integrazione WiFiManager per gestione parametri WiFi (MODALITA' BLOCCANTE)");

  // reset settings - for testing
  //resetWiFiConfig();

  // Mostra il MAC Address
  Serial.println();
  Serial.print("MAC Address: ");
  Serial.println(WiFi.macAddress());

  // Sposta lo stack rete nel task dedicato pinato al core 0
  Serial.println("\n[Setup] Avvio task WiFiManager su core 0...\n");
  if (!startWiFiManagerTask()) {
    Serial.println("[Setup] Errore avvio task WiFiManager");
    ESP.restart();
  }

  if (!waitWiFiManagerTaskInit(240000)) {
    Serial.println("[Setup] Timeout inizializzazione rete su core 0");
    ESP.restart();
  }

  Serial.println("\n[Setup] Stack rete inizializzato su core 0\n");
}

void loop()
{
  // Nessuna chiamata a API WiFi nel loop (core 1): stato pilotato dagli eventi
  digitalWrite(pinWiFiConnected, wifiConnectedFlag);
  vTaskDelay(pdMS_TO_TICKS(100));
}