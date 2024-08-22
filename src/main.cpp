#include <Arduino.h>
#include <WiFiManager.h> //https://github.com/tzapu/WiFiManager WiFi Configuration Magic
#include <ArduinoLog.h>

// Local Libraries
#include "restartReason.h"

#include "main.h"
#include "config.h"

WiFiManager wifiManager;

void setup()
{
  // Launch serial for debugging purposes
  Serial.begin(SERIAL_BAUD);
  Log.begin(LOG_LEVEL, &Serial);
  Log.notice(F(CR "************* WELCOME TO esp32_balboa_panel **************" CR));
  Log.notice(F("Version: %s" CR), VERSION);
  Log.notice(F("Build: %s" CR), BUILD);
  Log.notice(F("Last restart reason: %s" CR), getLastRestartReason().c_str());
  Log.verbose(F("Free heap: %d bytes" CR), ESP.getFreeHeap());
  Log.verbose(F("Free sketch space: %d bytes" CR), ESP.getFreeSketchSpace());
  Log.verbose(F("Chip ID: %d" CR), ESP.getEfuseMac());

  Log.verbose(F("Flash chip size: %d bytes" CR), ESP.getFlashChipSize());
  Log.verbose(F("Flash chip speed: %d Hz" CR), ESP.getFlashChipSpeed());
  Log.verbose(F("CPU frequency: %d Hz" CR), ESP.getCpuFreqMHz());
  Log.verbose(F("SDK version: %s" CR), ESP.getSdkVersion());


  Log.notice(F(CR "************* Setup Complete **************" CR));
}

void loop()
{
  // put your main code here, to run repeatedly:
}