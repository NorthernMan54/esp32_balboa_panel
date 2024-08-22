#include "wifiModule.h"

WiFiManager wifiManager;
char gatewayName[20];

void wifiModuleSetup()
{

  String s = WiFi.macAddress();
  sprintf(gatewayName, "spa-%.2s%.2s%.2s%.2s%.2s%.2s", s.c_str(),
          s.c_str() + 3, s.c_str() + 6, s.c_str() + 9, s.c_str() + 12,
          s.c_str() + 15);

  WiFi.setTxPower(WIFI_POWER_19_5dBm); // this sets wifi to highest power
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  unsigned long timeout = millis() + 10000;

  while (WiFi.status() != WL_CONNECTED && millis() < timeout)
  {
    yield();
  }

  configTime(gmtOffset_sec, daylightOffset_sec, "pool.ntp.org");
}

void wifiModuleLoop()
{
}