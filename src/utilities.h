#ifndef UTILITIES_H
#define UTILITIES_H

/**
 * Calculate uptime and take into account the millis() rollover
 * returns: unsigned long uptime in seconds
 */
unsigned long uptime();
#endif