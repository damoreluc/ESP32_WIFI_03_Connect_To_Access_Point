/**
 * @file wifi_manager_task.h
 * @brief Task FreeRTOS dedicato a WiFiManager sul core 0
 * 
 * Questo file fornisce un wrapper per eseguire handleWiFiManagerLoop()
 * in un task FreeRTOS separato, evitando di bloccare il loop principale
 */

#ifndef _WIFI_MANAGER_TASK_H
#define _WIFI_MANAGER_TASK_H

#include <Arduino.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

/**
 * @brief Avvia il task FreeRTOS per WiFiManager sul core 0
 * 
 * Il task utilizza uno stack di 20 KB (consigliato per wm.process())
 * Priority: 1 (bassa, per non interferire con altri task)
 * 
 * @return true se il task è stato creato con successo
 */
bool startWiFiManagerTask();

/**
 * @brief Attende il completamento dell'inizializzazione WiFiManager sul core 0
 * 
 * @param timeoutMs Timeout in millisecondi (0 = attesa infinita)
 * @return true se inizializzazione completata e rete connessa
 */
bool waitWiFiManagerTaskInit(uint32_t timeoutMs = 0);

/**
 * @brief Ferma il task FreeRTOS di WiFiManager
 */
void stopWiFiManagerTask();

/**
 * @brief Restituisce lo stato del task
 * 
 * @return true se il task è in esecuzione
 */
bool isWiFiManagerTaskRunning();

/**
 * @brief Restituisce true se lo stack di rete risulta pronto
 */
bool isNetworkStackReady();

/**
 * @brief Ottiene lo stack free del task WiFiManager
 * 
 * Utile per monitorare se lo stack è sufficiente
 * 
 * @return Numero di bytes liberi nello stack
 */
UBaseType_t getWiFiManagerTaskStackFree();

#endif
