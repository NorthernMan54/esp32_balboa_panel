#ifndef UTILITIES_H
#define UTILITIES_H
#include <Arduino.h>
/**
 * Calculate uptime and take into account the millis() rollover
 * returns: unsigned long uptime in seconds
 */
unsigned long uptime();
String msgToString(uint8_t *data, uint8_t len);

#endif