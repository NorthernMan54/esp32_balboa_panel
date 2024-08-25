#include <Arduino.h>

/**
 * Calculate uptime and take into account the millis() rollover
 * returns: unsigned long uptime in seconds
 */
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