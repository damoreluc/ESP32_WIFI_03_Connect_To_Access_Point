#include <WIFI/wifi_functions.h>
#include <HWCONFIG/hwConfig.h>

void initWiFi_STA()
{
  Serial.printf("[DIAG] initWiFi_STA() running on core %d\n", xPortGetCoreID());
#ifdef ARDUINO_RUNNING_CORE
  Serial.printf("[DIAG] ARDUINO_RUNNING_CORE (user loop/task) = %d\n", ARDUINO_RUNNING_CORE);
#endif
#ifdef ARDUINO_EVENT_RUNNING_CORE
  Serial.printf("[DIAG] ARDUINO_EVENT_RUNNING_CORE (WiFi events) = %d\n", ARDUINO_EVENT_RUNNING_CORE);
#endif

  // Timeout: max 20 secondi per connessione
  unsigned long startTime = millis();
  const unsigned long timeout = 20000; // 20 sec
  unsigned int attempts = 0;

  // comanda un led per indicare la connessione all'access point WiFi
  pinMode(pinWiFiConnected, OUTPUT);

  WiFi.mode(WIFI_STA);

  // registra solo gli eventi necessari (evita ARDUINO_EVENT_MAX)
  WiFi.onEvent(WiFiEvent, WiFiEvent_t::ARDUINO_EVENT_WIFI_STA_START);
  WiFi.onEvent(WiFiEvent, WiFiEvent_t::ARDUINO_EVENT_WIFI_STA_CONNECTED);
  WiFi.onEvent(WiFiEvent, WiFiEvent_t::ARDUINO_EVENT_WIFI_STA_DISCONNECTED);
  WiFi.onEvent(WiFiEvent, WiFiEvent_t::ARDUINO_EVENT_WIFI_STA_GOT_IP);
  WiFi.onEvent(WiFiEvent, WiFiEvent_t::ARDUINO_EVENT_WIFI_STA_LOST_IP);

  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.println(F("Connecting to WiFi "));

  // tenta la connessione, con timeout
  while (WiFi.status() != WL_CONNECTED && millis() - startTime < timeout)
  {
    Serial.print('.');
    vTaskDelay(pdMS_TO_TICKS(1000));
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