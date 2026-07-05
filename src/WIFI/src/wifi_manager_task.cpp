/**
 * @file wifi_manager_task.cpp
 * @brief Implementazione del task FreeRTOS per WiFiManager
 * 
 * Stack allocation per wm.process():
 * - Minimo richiesto: 12 KB
 * - Consigliato: 20 KB (usato qui)
 * - Massima sicurezza: 24 KB
 * 
 * Core: 0 (core dedicato)
 * Priority: 1 (bassa priorità)
 * 
 * La misurazione dello stack è stata fatta empiricamente:
 * - Parsing richieste HTTP: ~2-3 KB
 * - Buffer HTML risposta: ~4-5 KB
 * - Elaborazione parametri MQTT: ~1-2 KB
 * - Overhead FreeRTOS: ~3-4 KB
 * Total peak: ~12-16 KB (allocati 20 KB per sicurezza)
 */

#include <WIFI/wifi_manager_task.h>
#include <WIFI/wifi_manager.h>

// Handle del task
static TaskHandle_t wifiManagerTaskHandle = NULL;
static bool taskRunning = false;
static volatile bool wifiInitCompleted = false;
static volatile bool networkStackReady = false;

// Dimensione stack in bytes (20 KB consigliato per wm.process())
#define WIFI_MANAGER_STACK_SIZE 20480  // 20 KB

// Priority del task (1 = bassa, non interferisce con altre operazioni)
#define WIFI_MANAGER_TASK_PRIORITY 1

/**
 * @brief Task FreeRTOS che esegue handleWiFiManagerLoop()
 */
void wifiManagerTaskFunction(void *pvParameters)
{
  Serial.printf("[WiFiTask] Task WiFiManager avviato sul core %d\n", xPortGetCoreID());

  // Inizializzazione completa dello stack WiFi sul core 0
  initWiFiManager("ESP32-WiFi", 180);

  wifiInitCompleted = true;
  networkStackReady = (WiFi.status() == WL_CONNECTED);
  taskRunning = false;

  Serial.printf("[WiFiTask] Inizializzazione completata sul core %d\n", xPortGetCoreID());
  wifiManagerTaskHandle = NULL;
  vTaskDelete(NULL);
}

/**
 * @brief Avvia il task FreeRTOS per WiFiManager
 */
bool startWiFiManagerTask()
{
  if (taskRunning)
  {
    Serial.println(F("[WiFiTask] Task già in esecuzione"));
    return false;
  }
  
  wifiInitCompleted = false;
  networkStackReady = false;
  taskRunning = true;
  
  // Crea il task sul core 0
  BaseType_t result = xTaskCreatePinnedToCore(
    wifiManagerTaskFunction,        // Funzione del task
    "WiFiManagerTask",              // Nome del task (debug)
    WIFI_MANAGER_STACK_SIZE,        // Stack size in bytes (20 KB)
    NULL,                           // Parametri (nessuno)
    WIFI_MANAGER_TASK_PRIORITY,     // Priority (1 = bassa)
    &wifiManagerTaskHandle,         // Handle di output
    0                               // Core 0
  );
  
  if (result != pdPASS)
  {
    Serial.println(F("[WiFiTask] Errore: impossibile creare il task"));
    taskRunning = false;
    return false;
  }
  
  Serial.println(F("[WiFiTask] Task creato con successo"));
  Serial.print(F("[WiFiTask] Stack allocato: "));
  Serial.print(WIFI_MANAGER_STACK_SIZE);
  Serial.println(F(" bytes"));
  
  return true;
}

/**
 * @brief Ferma il task FreeRTOS di WiFiManager
 */
void stopWiFiManagerTask()
{
  if (!taskRunning)
  {
    Serial.println(F("[WiFiTask] Task non è in esecuzione"));
    return;
  }
  
  Serial.println(F("[WiFiTask] Arresto task WiFiManager..."));
  taskRunning = false;
  
  // Attende che il task termini
  if (wifiManagerTaskHandle != NULL)
  {
    // Attende max 2 secondi
    int timeout = 20;
    while (eTaskGetState(wifiManagerTaskHandle) != eDeleted && timeout-- > 0)
    {
      vTaskDelay(pdMS_TO_TICKS(100));
    }
    
    if (timeout <= 0)
    {
      Serial.println(F("[WiFiTask] Timeout durante l'arresto"));
    }
    else
    {
      Serial.println(F("[WiFiTask] Task fermato correttamente"));
    }
    
    wifiManagerTaskHandle = NULL;
  }
}

/**
 * @brief Restituisce lo stato del task
 */
bool isWiFiManagerTaskRunning()
{
  return taskRunning && (wifiManagerTaskHandle != NULL);
}

/**
 * @brief Attende il completamento dell'inizializzazione WiFiManager
 */
bool waitWiFiManagerTaskInit(uint32_t timeoutMs)
{
  unsigned long start = millis();

  while (!wifiInitCompleted)
  {
    if (timeoutMs > 0 && (millis() - start) >= timeoutMs)
    {
      return false;
    }
    vTaskDelay(pdMS_TO_TICKS(10));
  }

  return networkStackReady;
}

/**
 * @brief Restituisce lo stato della rete dopo init WiFiManager
 */
bool isNetworkStackReady()
{
  return networkStackReady;
}

/**
 * @brief Ottiene lo stack free del task WiFiManager
 */
UBaseType_t getWiFiManagerTaskStackFree()
{
  if (wifiManagerTaskHandle == NULL)
  {
    Serial.println(F("[WiFiTask] Task non è stato creato"));
    return 0;
  }
  
  UBaseType_t stackFree = uxTaskGetStackHighWaterMark(wifiManagerTaskHandle);
  return stackFree;
}

/**
 * @brief Stampa informazioni debug del task WiFiManager
 */
void printWiFiManagerTaskDebug()
{
  if (wifiManagerTaskHandle == NULL)
  {
    Serial.println(F("[WiFiTask] Task non è stato creato"));
    return;
  }
  
  Serial.println(F("\n=== WiFiManager Task Debug Info ==="));
  Serial.print(F("Task Handle: 0x"));
  Serial.println((uintptr_t)wifiManagerTaskHandle, HEX);
  
  Serial.print(F("Task State: "));
  eTaskState state = eTaskGetState(wifiManagerTaskHandle);
  switch (state)
  {
  case eRunning:
    Serial.println(F("Running"));
    break;
  case eReady:
    Serial.println(F("Ready"));
    break;
  case eBlocked:
    Serial.println(F("Blocked"));
    break;
  case eSuspended:
    Serial.println(F("Suspended"));
    break;
  case eDeleted:
    Serial.println(F("Deleted"));
    break;
  default:
    Serial.println(F("Unknown"));
    break;
  }
  
  Serial.print(F("Stack Size: "));
  Serial.print(WIFI_MANAGER_STACK_SIZE);
  Serial.println(F(" bytes"));
  
  Serial.print(F("Stack High Water Mark: "));
  Serial.print(uxTaskGetStackHighWaterMark(wifiManagerTaskHandle));
  Serial.println(F(" bytes"));
  
  Serial.print(F("Priority: "));
  Serial.println(uxTaskPriorityGet(wifiManagerTaskHandle));
  
  Serial.println(F("===================================\n"));
}
