#include <WIFI/wifi_functions.h>
#include <HWCONFIG/hwConfig.h>

bool initWiFi_STA()
{
  pinMode(pinWiFiConnected, OUTPUT);
  digitalWrite(pinWiFiConnected, LOW);
  wifiConnectedFlag = false;

  WiFi.mode(WIFI_STA);
  WiFi.disconnect(true, true);

  WiFi.onEvent(WiFiEvent, ARDUINO_EVENT_WIFI_STA_CONNECTED);
  WiFi.onEvent(WiFiEvent, ARDUINO_EVENT_WIFI_STA_DISCONNECTED);
  WiFi.onEvent(WiFiGotIP, ARDUINO_EVENT_WIFI_STA_GOT_IP);
  WiFi.onEvent(WiFiEvent, ARDUINO_EVENT_WIFI_STA_LOST_IP);
  WiFi.onEvent(WiFiStationDisconnected, ARDUINO_EVENT_WIFI_STA_DISCONNECTED);

  WiFi.begin();
  Serial.println(F("Connecting to saved WiFi credentials"));

  unsigned long startTime = millis();
  const unsigned long timeout = 20000;
  unsigned int attempts = 0;

  while (WiFi.status() != WL_CONNECTED && millis() - startTime < timeout) {
    Serial.print('.');
    vTaskDelay(pdMS_TO_TICKS(500));
    attempts++;
  }

  if (WiFi.status() != WL_CONNECTED) {
    Serial.println(F("\n[ERROR] WiFi connection timeout"));
    digitalWrite(pinWiFiConnected, LOW);
    return false;
  }

  Serial.printf("\n[OK] WiFi connected in %u attempts\n", attempts);
  Serial.print(F("[OK] IP: "));
  Serial.println(WiFi.localIP());
  wifiConnectedFlag = true;
  digitalWrite(pinWiFiConnected, HIGH);
  return true;
}