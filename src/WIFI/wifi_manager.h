#ifndef _WIFI_MANAGER_H
#define _WIFI_MANAGER_H

#include <Arduino.h>
#include <WiFiManager.h>
#include <WiFi.h>

/**
 * @brief Inizializza e configura WiFiManager per la gestione automatica dei parametri WiFi
 * 
 * Se la ESP32 non è mai stata configurata o le credenziali salvate sono non valide,
 * WiFiManager avvierà un Access Point temporaneo (AutoConnect) su cui l'utente 
 * potrà connettersi e configurare le credenziali WiFi tramite una pagina web.
 * 
 * Le credenziali vengono salvate automaticamente in flash memory per connessioni future.
 * 
 * @param deviceName Nome del dispositivo (usato come SSID dell'AP temporaneo)
 * @param timeoutSeconds Timeout massimo per la configurazione (default 180 sec)
 */
void initWiFiManager(const char* deviceName = "ESP32-WiFi", uint32_t timeoutSeconds = 180);

/**
 * @brief Disconnette da WiFi e cancella le credenziali salvate
 * 
 * Utile per resettare il dispositivo e riconfigurare da zero
 */
void resetWiFiConfig();

/**
 * @brief Gestisce gli eventi WiFi collegati a WiFiManager
 * 
 * @param event Tipo di evento WiFi
 */
void WiFiManagerEvent(WiFiEvent_t event);

/**
 * @brief Funzione da chiamare nel loop() per mantenere attivo il captive portal
 * 
 * Gestisce il DNS server e il web server del captive portal
 */
void handleWiFiManagerLoop();

/**
 * @brief Restituisce l'indirizzo del broker MQTT salvato nel captive portal
 * 
 * @return Indirizzo del broker MQTT (es: "mqtt.example.com")
 */
const char* getMQTTBroker();

/**
 * @brief Restituisce la password MQTT salvata nel captive portal
 * 
 * @return Password MQTT
 */
const char* getMQTTPassword();

/**
 * @brief Restituisce il nome del client MQTT salvato nel captive portal
 * 
 * @return Nome del client MQTT
 */
const char* getMQTTClientName();

#endif
