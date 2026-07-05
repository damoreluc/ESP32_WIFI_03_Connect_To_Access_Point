#include <WIFI/wifi_functions.h>

void WiFiGotIP(WiFiEvent_t event, WiFiEventInfo_t info)
{
    wifiConnectedFlag = true;
    Serial.print(F("[WiFi] IP obtained: "));
    Serial.println(WiFi.localIP());
    WiFiNetworkReady();
}