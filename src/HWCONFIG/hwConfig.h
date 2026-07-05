#ifndef __HWCONFIG_H
#define __HWCONFIG_H

#include <Arduino.h>

// input pin


// output pin


// indicatore stato connessione WiFi
#define pinWiFiConnected GPIO_NUM_23

#if (pinWiFiConnected < 0 || pinWiFiConnected > 39)
  #error "[ERROR] Invalid GPIO pin for WiFi LED"
#endif

#endif
