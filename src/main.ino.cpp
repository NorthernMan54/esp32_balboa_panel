# 1 "/var/folders/nk/2b522cbn5pdfsm1_n0zm50680000gp/T/tmpo84c416d"
#include <Arduino.h>
# 1 "/Users/sgracey/Code/esp32_balboa_panel/src/main.ino"
#include <Arduino.h>
#include <WiFiManager.h>
#include <ArduinoLog.h>
#include <esp_task_wdt.h>


#include <restartReason.h>
#include <wifiModule.h>
#include <findSpa.h>
#include <spaCommunication.h>
#include <spaMessage.h>
#include <spaWebServer.h>
#include "utilities.h"

#include "main.h"
void setup();
void loop();
unsigned long uptime();
unsigned long getTime();
String msgToString(uint8_t *data, uint8_t len);
void append_request(unsigned char *byte_array, int *offset, unsigned char *request, int request_size);
String formatNumberWithCommas(uint32_t num);
String formatNumberWithCommas(unsigned long num);
void _printTimestamp(Print* _logOutput);
void _printLogLevel(Print* _logOutput, int logLevel);
void printPrefix(Print* _logOutput, int logLevel);
#line 17 "/Users/sgracey/Code/esp32_balboa_panel/src/main.ino"
void setup()
{

  Serial.begin(SERIAL_BAUD);
  Log.setPrefix(printPrefix);
  Log.begin(LOG_LEVEL, &Serial);
  esp_task_wdt_init(INITIAL_WDT_TIMEOUT, true);
  esp_task_wdt_add(NULL);
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
  logSection("Spa Message Setup");
  spaMessageSetup();
  logSection("Web Server Setup");
  spaWebServerSetup();
  logSection("Setup Complete");
}

void loop()
{
  wifiModuleLoop();
  if (WiFi.status() == WL_CONNECTED)
  {
    if (findSpaLoop())
    {
      if (!spaCommunicationLoop(getSpaIP()))
      {
        Log.verbose(F("[Main]: spaCommunicationLoop failed, client disconnected" CR));
        resetSpaCount();
      }
      spaMessageLoop();
      spaWebServerLoop();
    }
  }
}
# 1 "/Users/sgracey/Code/esp32_balboa_panel/src/utilities.ino"
#include <Arduino.h>





unsigned long uptime() {
  static unsigned long lastUptime = 0;
  static unsigned long uptimeAdd = 0;
  unsigned long uptime = millis() / 1000 + uptimeAdd;
  if (uptime < lastUptime) {
    uptime += 4294967;
    uptimeAdd += 4294967;
  }
  lastUptime = uptime;
  return uptime;
}

unsigned long getTime() {
  time_t now;
  struct tm timeinfo;
  if (!getLocalTime(&timeinfo)) {

    return(0);
  }
  time(&now);
  return now;
}

String msgToString(uint8_t *data, uint8_t len) {
  String s;
  for (int i = 0; i < len; i++) {
    if (data[i] < 0x10)
      s += "0";
    s += String(data[i], HEX);
    s += " ";
  }
  return s;
}

void append_request(unsigned char *byte_array, int *offset, unsigned char *request, int request_size) {
    memcpy(byte_array + *offset, request, request_size);
    *offset += request_size;
}

String formatNumberWithCommas(uint32_t num) {
  return formatNumberWithCommas((unsigned long)num);
}

String formatNumberWithCommas(unsigned long num) {
    String numStr = String(num);
    int insertPosition = numStr.length() - 3;

    while (insertPosition > 0) {
        numStr = numStr.substring(0, insertPosition) + ',' + numStr.substring(insertPosition);
        insertPosition -= 3;
    }

    return numStr;
}



void _printTimestamp(Print* _logOutput) {


  const unsigned long MSECS_PER_SEC = 1000;
  const unsigned long SECS_PER_MIN = 60;
  const unsigned long SECS_PER_HOUR = 3600;
  const unsigned long SECS_PER_DAY = 86400;


  const unsigned long msecs = millis();
  const unsigned long secs = msecs / MSECS_PER_SEC;


  const unsigned long MilliSeconds = msecs % MSECS_PER_SEC;
  const unsigned long Seconds = secs % SECS_PER_MIN ;
  const unsigned long Minutes = (secs / SECS_PER_MIN) % SECS_PER_MIN;
  const unsigned long Hours = (secs % SECS_PER_DAY) / SECS_PER_HOUR;


  char timestamp[20];
  sprintf(timestamp, "%02d:%02d:%02d.%03d ", Hours, Minutes, Seconds, MilliSeconds);
  _logOutput->print(timestamp);
}

void _printLogLevel(Print* _logOutput, int logLevel) {

    switch (logLevel)
    {
        default:
        case 0:_logOutput->print("SILENT " ); break;
        case 1:_logOutput->print("FATAL " ); break;
        case 2:_logOutput->print("ERROR " ); break;
        case 3:_logOutput->print("WARNING "); break;
        case 4:_logOutput->print("INFO " ); break;
        case 5:_logOutput->print("TRACE " ); break;
        case 6:_logOutput->print("VERBOSE "); break;
    }
}

void printPrefix(Print* _logOutput, int logLevel) {
    _printTimestamp(_logOutput);

}