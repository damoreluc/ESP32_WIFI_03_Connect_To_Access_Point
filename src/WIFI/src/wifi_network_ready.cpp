#include <WIFI/wifi_functions.h>
#include <HWCONFIG/hwConfig.h>

// Operazioni dei layer superiori da compiere quando è pronto il layer IP
void WiFiNetworkReady()
{
    digitalWrite(pinWiFiConnected, HIGH);

}
