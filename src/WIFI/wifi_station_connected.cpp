#include <WIFI/wifi_functions.h>

void WiFiStationConnected(WiFiEvent_t event, WiFiEventInfo_t info)
{
    Serial.println(F("Connected to AP successfully!"));
}