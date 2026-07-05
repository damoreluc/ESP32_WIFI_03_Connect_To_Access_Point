#ifndef _WIFI_FUNCTIONS_H
#define _WIFI_FUNCTIONS_H

#include <Arduino.h>
#include <WIFI/credentials.h>
#include <WiFi.h>

// configura il modulo WiFi come STATION
// e si connette ad un Access Point con le credenziali 
// definite nel file credentials.h
void initWiFi_STA();

// Gestione degli eventi del WiFi
void WiFiEvent(WiFiEvent_t event, WiFiEventInfo_t info);

// Operazioni dei layer superiori da compiere quando è pronto il layer IP
void WiFiNetworkReady();

// Operazioni da compiere sui layers superiori se il layer IP è caduto
void WiFiNetworkFail();

#endif
