#include <WIFI/wifi_functions.h>
#include <HWCONFIG/hwConfig.h>

static unsigned long lastReconnectTime = 0;
static unsigned int reconnectAttempts = 0;
static unsigned long reconnectBackoffMs = 1000;
const unsigned int maxReconnectAttempts = 8;
const unsigned long maxBackoffMs = 30000;

// tabella dei codici reason in:
// https://community.cisco.com/t5/wireless-mobility-knowledge-base/802-11-association-status-802-11-deauth-reason-codes/ta-p/3148055
void WiFiStationDisconnected(WiFiEvent_t event, WiFiEventInfo_t info)
{
    Serial.print(F("[WiFi] Lost connection. Reason: "));
    Serial.println(info.wifi_sta_disconnected.reason);

    unsigned long now = millis();
    if (now - lastReconnectTime < reconnectBackoffMs) {
        Serial.printf("[WiFi] Waiting %lu ms before reconnecting...\n", reconnectBackoffMs - (now - lastReconnectTime));
        return;
    }

    if (reconnectAttempts >= maxReconnectAttempts) {
        Serial.println(F("[WiFi] Max reconnect attempts reached. Restarting ESP32"));
        delay(1000);
        ESP.restart();
        return;
    }

    Serial.printf("[WiFi] Reconnecting (%u/%u)...\n", reconnectAttempts + 1, maxReconnectAttempts);
    WiFi.reconnect();
    lastReconnectTime = now;
    reconnectAttempts++;
    reconnectBackoffMs = min(reconnectBackoffMs * 2, maxBackoffMs);
}