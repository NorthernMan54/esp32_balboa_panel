#include <Arduino.h>
#include <WiFiManager.h> //https://github.com/tzapu/WiFiManager WiFi Configuration Magic
#include <ArduinoLog.h>
#include <esp_task_wdt.h>

// Local Libraries
#include <restartReason.h>
#include <wifiModule.h>
#include <findSpa.h>
#include <spaCommunication.h>

#include "main.h"

void setup()
{
  // Launch serial for debugging purposes
  Serial.begin(SERIAL_BAUD);
  Log.begin(LOG_LEVEL, &Serial);
  esp_task_wdt_init(INITIAL_WDT_TIMEOUT, true); // enable panic so ESP32 restarts
  esp_task_wdt_add(NULL);                       // add current thread to WDT watch
  logSection("WELCOME TO esp32_balboa_panel");
  Log.notice(F("Version: %s" CR), VERSION);
  Log.notice(F("Build: %s" CR), BUILD);
  Log.notice(F("Last restart reason: %s" CR), getLastRestartReason().c_str());
  Log.verbose(F("Free heap: %d bytes" CR), ESP.getFreeHeap());
  Log.verbose(F("Free sketch space: %d bytes" CR), ESP.getFreeSketchSpace());
  Log.verbose(F("Chip ID: %x" CR), ESP.getEfuseMac());

  Log.verbose(F("Flash chip size: %d bytes" CR), ESP.getFlashChipSize());
  Log.verbose(F("Flash chip speed: %d Hz" CR), ESP.getFlashChipSpeed());
  Log.verbose(F("CPU frequency: %d Hz" CR), ESP.getCpuFreqMHz());
  Log.verbose(F("SDK version: %s" CR), ESP.getSdkVersion());

  logSection("Wifi Module Setup");
  wifiModuleSetup();
  logSection("Find Spa Setup");
  findSpaSetup();
 logSection("Spa Communications Setup");
  spaCommunicationSetup();
  logSection("Setup Complete");
}

void loop()
{
  wifiModuleLoop();
  if (WiFi.status() == WL_CONNECTED)
  {
    findSpaLoop();
    if (spaFound())
    {
      spaCommunicationLoop(getSpaIP());
    }
    esp_task_wdt_reset();
  }
  // put your main code here, to run repeatedly:
}