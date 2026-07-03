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
  Serial.println("\n\nWiFi Esempio 03: connessione ad un Access Point e gestione riconnessione automatica");
  Serial.println("Con integrazione WiFiManager per gestione parametri WiFi");

  // reset settings - for testing
  //resetWiFiConfig();

  // Mostra il MAC Address
  Serial.println();
  Serial.print("MAC Address: ");
  Serial.println(WiFi.macAddress());

  // Inizializza WiFiManager con parametri personalizzati
  // - Nome del dispositivo per l'AP temporaneo: "ESP32-WiFi"
  // - Timeout di configurazione: 180 secondi
  initWiFiManager("ESP32-WiFi", 180);

  // Avvia il task FreeRTOS di WiFiManager sul core 0
  if (startWiFiManagerTask())
  {
    Serial.println("[Main] WiFiManager task avviato con successo");
  }
  else
  {
    Serial.println("[Main] Errore nel lancio del task");
  } 
}

void loop()
{
  // Monitoraggio dello stack del task WiFiManager
  if (isWiFiManagerTaskRunning())
  {
    UBaseType_t stackFree = getWiFiManagerTaskStackFree();
    if (stackFree < 2048)  // Meno di 2 KB liberi
    {
      Serial.print("[WARNING] Stack WiFiManager basso: ");
      Serial.print(stackFree);
      Serial.println(" bytes");
    }
  }
  
  // Codice da eseguire ciclicamente...

  vTaskDelay(pdMS_TO_TICKS(100));
}