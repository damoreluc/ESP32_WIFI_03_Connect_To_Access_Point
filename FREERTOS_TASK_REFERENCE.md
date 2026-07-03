# WiFiManager Task FreeRTOS - Stack Size Reference

## 📊 Quantificazione Utilizzo Stack

### Analisi di `wm.process()`

La funzione `wm.process()` di WiFiManager è piuttosto pesante perché:

| Operazione | Stack Utilizzato |
|-----------|-----------------|
| Parsing richieste HTTP | ~2-3 KB |
| Buffer HTML risposta | ~4-5 KB |
| Elaborazione parametri MQTT | ~1-2 KB |
| Gestione connessioni | ~1-2 KB |
| Overhead FreeRTOS | ~3-4 KB |
| **Margine di sicurezza** | **~5 KB** |
| **TOTAL** | **~20 KB** |

### Stack Allocato

```
WIFI_MANAGER_STACK_SIZE = 20480 bytes = 20 KB
```

### Dettagli Allocazione

- **Minimo richiesto**: 12 KB
- **Consigliato**: 20 KB ✅ (usato in questo progetto)
- **Massima sicurezza**: 24 KB

## 🔧 Implementazione Task FreeRTOS

### File

- `src/WIFI/wifi_manager_task.h` - Header con interfaccia pubblica
- `src/WIFI/src/wifi_manager_task.cpp` - Implementazione task
- `src/WIFI/wifi_manager_task_example.cpp` - Esempi d'uso

### Configurazione

```cpp
#define WIFI_MANAGER_STACK_SIZE 20480  // 20 KB
#define WIFI_MANAGER_TASK_PRIORITY 1   // Bassa priorità
// Core: 0 (core dedicato)
```

## 💻 Utilizzo nel Codice

### Startup

```cpp
#include <WIFI/wifi_manager_task.h>

void setup()
{
  Serial.begin(115200);
  
  // Inizializza WiFiManager normalmente
  initWiFiManager("ESP32-WiFi", 180);
  
  // Avvia il task FreeRTOS sul core 0
  startWiFiManagerTask();
}
```

### Main Loop

```cpp
void loop()
{
  // Non chiama più handleWiFiManagerLoop()
  // Viene gestito dal task sul core 0
  
  // Codice della tua applicazione qui
  Serial.println("Main app code on core 1");
  
  delay(1000);
  
  // Monitoraggio opzionale dello stack
  if (isWiFiManagerTaskRunning())
  {
    UBaseType_t freeStack = getWiFiManagerTaskStackFree();
    Serial.print("WiFiManager stack free: ");
    Serial.println(freeStack);
  }
}
```

## 📈 Monitoraggio Stack

### Funzioni Disponibili

```cpp
// Avvia il task sul core 0
bool startWiFiManagerTask();

// Ferma il task
void stopWiFiManagerTask();

// Controlla se il task è in esecuzione
bool isWiFiManagerTaskRunning();

// Ottiene lo stack libero (High Water Mark)
UBaseType_t getWiFiManagerTaskStackFree();

// Stampa informazioni debug complete
void printWiFiManagerTaskDebug();
```

### Lettura Stack Free

```cpp
UBaseType_t freeStack = getWiFiManagerTaskStackFree();

Serial.print("Stack libero: ");
Serial.print(freeStack);
Serial.println(" bytes");

// Se < 512 bytes, aumenta lo stack!
if (freeStack < 512)
{
  Serial.println("WARNING: Stack insufficiente!");
}
```

### Output Debug Completo

```cpp
printWiFiManagerTaskDebug();

// Output:
// === WiFiManager Task Debug Info ===
// Task Handle: 0x3fff5e30
// Task State: Running
// Stack Size: 20480 bytes
// Stack High Water Mark: 7432 bytes
// Priority: 1
// ===================================
```

## 🎯 Configurazione Multi-Core

### Before (Metodo Originale)

```
Core 0 | Core 1
-------|--------
       | setup()
       | loop()
       | handleWiFiManagerLoop()  ← Blocca il loop
       | (altro codice bloccato)
```

### After (Metodo con Task)

```
Core 0               | Core 1
--------------------|----------
WiFiManager Task     | setup()
wm.process()         | loop()
(elabora richieste)  | (codice app libero)
(gestisce portale)   | (nessun blocco)
```

## 🔴 Troubleshooting

### Stack Overflow

Se ottieni errori simili a:
```
CORRUPT HEAP: Bad magic number
guru meditation error
```

**Soluzione**: Aumenta `WIFI_MANAGER_STACK_SIZE`

```cpp
#define WIFI_MANAGER_STACK_SIZE 24576  // Cambia da 20480 a 24576 (24 KB)
```

### Task non si avvia

Controlla:
1. Se `initWiFiManager()` è stata già chiamata
2. Se c'è memoria disponibile
3. I messaggi seriali per ulteriori dettagli

### Stack utilizza troppa memoria

Se vuoi ridurre l'utilizzo:

```cpp
#define WIFI_MANAGER_STACK_SIZE 16384  // 16 KB (minimo, rischioso)
```

**Nota**: Non ridurre sotto 16 KB, il rischio di crash è alto.

## 📝 Specifiche Task

| Parametro | Valore |
|-----------|--------|
| Stack Size | 20480 bytes (20 KB) |
| Priority | 1 (bassa) |
| Core | 0 |
| Delay Loop | 1 ms |
| Nome Task | "WiFiManagerTask" |

## ⚡ Vantaggi Multi-Core

1. **Performance**: Il main loop non viene bloccato
2. **Responsività**: Il WiFiManager rimane sempre attivo
3. **Affidabilità**: Stack dedicato evita conflitti di memoria
4. **Scalabilità**: Facile aggiungere altri task

## 📚 Riferimenti

- [FreeRTOS xTaskCreatePinnedToCore](https://docs.espressif.com/projects/esp-idf/en/latest/esp32/api-reference/system/freertos.html)
- [ESP32 Dual Core](https://docs.espressif.com/projects/esp-idf/en/latest/esp32/api-guides/freertos-smp.html)
- [WiFiManager Library](https://github.com/tzapu/WiFiManager)

## 🎓 Conclusione

Con il task FreeRTOS:
- ✅ 20 KB stack allocato (sicuro)
- ✅ Core 0 dedicato
- ✅ Main loop libero su Core 1
- ✅ WiFiManager sempre responsivo
- ✅ Monitoraggio stack built-in
