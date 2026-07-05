/*
 * WiFi Esempio 03: connessione ad un Access Point e gestione riconnessione automatica
 *
 * L'esempio illustra l'insieme delle operazioni necessarie per collegare la ESP32 ad un Access Point WiFi
 * e gestire le situazioni di perdita di connessione e riconnessione automatica.
 *
 * Per il controllo dello stato della connessione vengono adoperati gli eventi dell'oggetto Wifi
 *
 */

#include <Arduino.h>
#include <HWCONFIG/hwConfig.h>
#include <WiFi.h>
#include <WIFI/wifi_functions.h>


void setup()
{
  Serial.begin(115200);
  Serial.println("WiFi Esempio 03: connessione ad un Access Point e gestione riconnessione automatica");
  Serial.printf("[DIAG] setup() running on core %d\n", xPortGetCoreID());

  // impiega il metodo macAddress() dell'oggetto WiFi
  Serial.println();
  Serial.print("MAC Address: ");
  Serial.println(WiFi.macAddress());

  // registra gli handler degli eventi WiFi e avvia la connessione ad un access point 
  // in modalità STATION 
  initWiFi_STA();
}

void loop()
{
  // stampa periodica per verificare il core del task utente (loopTask)
  static uint32_t lastDiagMs = 0;
  if (millis() - lastDiagMs >= 5000)
  {
    lastDiagMs = millis();
    Serial.printf("[DIAG] loop() running on core %d\n", xPortGetCoreID());
  }

  vTaskDelay(pdMS_TO_TICKS(100));
}