#ifndef _WIFI_FUNCTIONS_H
#define _WIFI_FUNCTIONS_H

#include <Arduino.h>
#include <WiFi.h>

// Flag globale per sincronizzare lo stato del LED con gli eventi WiFi
extern volatile bool wifiConnectedFlag;

// Flag globale per evitare duplicazioni della notifica "network ready"
extern bool g_wifiNetworkReadyNotified;

// configura il modulo WiFi come STATION
// e prova a connettersi usando le credenziali gia' salvate nel sistema WiFi
bool initWiFi_STA();

// Gestione degli eventi del WiFi
void WiFiEvent(WiFiEvent_t event);

// Stampa l'indirizzo IP ottenuto 
void WiFiGotIP(WiFiEvent_t event, WiFiEventInfo_t info);

// Stampa la conferma di connessione all'access point
void WiFiStationConnected(WiFiEvent_t event, WiFiEventInfo_t info);

// Gestione della disconnessione e tentativo di riconnessione
void WiFiStationDisconnected(WiFiEvent_t event, WiFiEventInfo_t info);

// Operazioni dei layer superiori da compiere quando è pronto il layer IP
void WiFiNetworkReady();

// Operazioni da compiere sui layers superiori se il layer IP è caduto
void WiFiNetworkFail();

#endif
