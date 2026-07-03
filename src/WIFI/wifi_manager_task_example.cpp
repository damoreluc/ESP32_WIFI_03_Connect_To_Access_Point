/**
 * @example Utilizzo del task FreeRTOS per WiFiManager
 * 
 * Questo esempio mostra come eseguire WiFiManager in un task FreeRTOS
 * separato sul core 0, lasciando libero il core 1 per il main loop
 */

#include <Arduino.h>
#include <WIFI/wifi_manager.h>
#include <WIFI/wifi_manager_task.h>

/*
// ESEMPIO 1: Utilizzo nel setup()
void setup()
{
  Serial.begin(115200);
  delay(1000);
  
  // Inizializza WiFiManager
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
  
  delay(2000);
}

// ESEMPIO 2: Loop principale - NON chiama handleWiFiManagerLoop()
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
  
  // Codice dell'applicazione principale
  // Questo gira sul core 1 senza interferenze dal WiFiManager
  Serial.println("[App] Core 1 - Main loop");
  
  // Stampa debug periodico (ogni 10 secondi)
  delay(10000);
  printWiFiManagerTaskDebug();
}

// ESEMPIO 3: Come ottenere i parametri MQTT
void setupMQTT()
{
  const char* broker = getMQTTBroker();
  const char* password = getMQTTPassword();
  const char* clientName = getMQTTClientName();
  
  Serial.print("MQTT Broker: ");
  Serial.println(broker);
  Serial.print("MQTT Client: ");
  Serial.println(clientName);
  
  // Connettiti al broker...
}

// ESEMPIO 4: Fermaggio del task
void disableWiFiManager()
{
  stopWiFiManagerTask();
}
*/

/**
 * ALLOCAZIONE DELLO STACK:
 * 
 * Stack Size: 20480 bytes (20 KB)
 * 
 * Breakdown:
 * - Parsing richieste HTTP: ~2-3 KB
 * - Buffer HTML risposta: ~4-5 KB
 * - Elaborazione parametri: ~1-2 KB
 * - Overhead FreeRTOS: ~3-4 KB
 * - Margine di sicurezza: ~5 KB
 * ─────────────────────────
 * TOTAL: ~20 KB
 * 
 * Se ottieni errori di stack overflow durante la configurazione WiFi,
 * aumenta WIFI_MANAGER_STACK_SIZE in wifi_manager_task.cpp a 24576 (24 KB)
 */

/**
 * CORE ALLOCATION:
 * 
 * Core 0: WiFiManager Task
 *         - Gestisce web server (192.168.4.1)
 *         - Processa richieste HTTP
 *         - Legge parametri MQTT
 *         - Priority: 1 (bassa)
 * 
 * Core 1: Main Loop (loop())
 *         - Esegue il codice dell'applicazione
 *         - Non viene bloccato da WiFiManager
 *         - Può fare altre operazioni senza interruzioni
 */

/**
 * MONITORAGGIO DELLO STACK:
 * 
 * Puoi controllare quanto stack è realmente usato:
 * 
 *   UBaseType_t freeStack = getWiFiManagerTaskStackFree();
 *   Serial.print("Stack libero: ");
 *   Serial.println(freeStack);
 * 
 * Il valore "High Water Mark" indica il picco minimo di stack
 * raggiunto durante l'esecuzione.
 * 
 * Se High Water Mark < 512 bytes, considera di aumentare lo stack.
 */

/**
 * CONFRONTO: CON TASK vs SENZA TASK
 * 
 * SENZA TASK (metodo originale):
 * ├─ loop() chiama handleWiFiManagerLoop()
 * ├─ Main loop è bloccato da wm.process()
 * └─ Potrebbero mancare evento di temporizzazione
 * 
 * CON TASK (metodo consigliato):
 * ├─ Core 0: Task WiFiManager con wm.process()
 * ├─ Core 1: Main loop libero per altre operazioni
 * └─ Tutto gira in parallelo senza blocchi
 */
