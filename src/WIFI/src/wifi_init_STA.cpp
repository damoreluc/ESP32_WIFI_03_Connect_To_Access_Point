#include <WIFI/wifi_functions.h>
#include <HWCONFIG/hwConfig.h>

void initWiFi_STA()
{
  // Timeout: max 20 secondi per connessione
  unsigned long startTime = millis();
  const unsigned long timeout = 20000; // 20 sec
  unsigned int attempts = 0;

  // comanda un led per indicare la connessione all'access point WiFi
  pinMode(pinWiFiConnected, OUTPUT);

  WiFi.mode(WIFI_STA);

  // gestione degli eventi WiFi
  WiFi.onEvent(WiFiEvent, WiFiEvent_t::ARDUINO_EVENT_MAX);
  // WiFi.onEvent(WiFiStationConnected, WiFiEvent_t::ARDUINO_EVENT_WIFI_STA_CONNECTED);
  // WiFi.onEvent(WiFiGotIP, WiFiEvent_t::ARDUINO_EVENT_WIFI_STA_GOT_IP);
  WiFi.onEvent(WiFiStationDisconnected, WiFiEvent_t::ARDUINO_EVENT_WIFI_STA_DISCONNECTED);

  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.println(F("Connecting to WiFi "));

  // tenta la connessione, con timeout
  while (WiFi.status() != WL_CONNECTED && millis() - startTime < timeout)
  {
    Serial.print('.');
    delay(1000);
    attempts++;
  }

  // connessione stabilita?
  if (WiFi.status() != WL_CONNECTED)
  {
    Serial.println("\n[ERROR] WiFi connection timeout!");
    delay(3000);
    ESP.restart();
  }
  else
  {
    Serial.printf(" \n connected in %d attemps\n", attempts);
  }
}