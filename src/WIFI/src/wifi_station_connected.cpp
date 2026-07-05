#include <WIFI/wifi_functions.h>
#include <HWCONFIG/hwConfig.h>

void WiFiStationConnected(WiFiEvent_t event, WiFiEventInfo_t info)
{
    wifiConnectedFlag = true;
    digitalWrite(pinWiFiConnected, HIGH);
    Serial.println(F("[WiFi] Connected to AP successfully!"));
}