#include <WIFI/wifi_functions.h>
#include <HWCONFIG/hwConfig.h>

// Operazioni da compiere sui layers superiori se il layer IP è caduto
void WiFiNetworkFail() {
    g_wifiNetworkReadyNotified = false;
    unsigned long now = millis();
    Serial.printf("\n[EVENT][%lu ms] WiFi Network FAILED - IP layer down\n", now);
    Serial.println(F("[TODO] Fallback actions: stop network operations, save data, enter offline mode"));
    digitalWrite(pinWiFiConnected, LOW);
}
