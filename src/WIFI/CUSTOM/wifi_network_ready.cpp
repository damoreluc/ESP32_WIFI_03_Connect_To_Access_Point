#include <WIFI/wifi_functions.h>
#include <HWCONFIG/hwConfig.h>
// Flag globale per evitare duplicazioni della notifica "network ready"
bool g_wifiNetworkReadyNotified = false;

// Operazioni dei layer superiori da compiere quando è pronto il layer IP
void WiFiNetworkReady()
{
    if (g_wifiNetworkReadyNotified) {
        return;
    }

    g_wifiNetworkReadyNotified = true;
    unsigned long now = millis();
    Serial.printf("\n[EVENT][%lu ms] WiFi Network is READY - IP layer operational\n", now);
    Serial.println(F("[TODO] Application-layer tasks: NTP sync, MQTT connect, cloud operations"));
    if (WiFi.status() == WL_CONNECTED) {
        Serial.printf("[OK][%lu ms] Connected to: %s\n", now, WiFi.SSID().c_str());
        Serial.printf("[OK][%lu ms] IP: %s\n", now, WiFi.localIP().toString().c_str());
        Serial.printf("[OK][%lu ms] RSSI: %d dBm\n", now, WiFi.RSSI());
    }
}
