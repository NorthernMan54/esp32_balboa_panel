#ifndef WIFI_MODULE_H
#define WIFI_MODULE_H

#include <Arduino.h>
#include <WiFi.h>
#include <WiFiManager.h>

#include "config.h" // Default passwords and SSID

#ifndef WIFI_SSID
#warning "WIFI_SSID not defined, please define in config.h"
#define WIFI_SSID "spa"
#endif

#ifndef WIFI_PASSWORD
#warning "WIFI_PASSWORD not defined, please define in config.h"
#define WIFI_PASSWORD "password"
#endif

#ifndef GMT_OFFSET
#warning "GMT_OFFSET not defined, please define in config.h"
#define GMT_OFFSET -14400
#endif

#ifndef DAYLIGHT_OFFSET
#warning "DAYLIGHT_OFFSET not defined, please define in config.h"
#define DAYLIGHT_OFFSET 0
#endif

const long gmtOffset_sec = GMT_OFFSET;
const int daylightOffset_sec = DAYLIGHT_OFFSET;

void wifiModuleSetup();
void wifiModuleLoop();

#endif