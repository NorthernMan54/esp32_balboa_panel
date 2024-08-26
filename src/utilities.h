#ifndef UTILITIES_H
#define UTILITIES_H
#include <Arduino.h>
/**
 * Calculate uptime and take into account the millis() rollover
 * returns: unsigned long uptime in seconds
 */
unsigned long uptime();
unsigned long getTime();
String msgToString(uint8_t *data, uint8_t len);
void append_request(unsigned char *byte_array, int *offset, unsigned char *request, int request_size);

#endif