#ifndef SPA_COMMUNICATION_H
#define SPA_COMMUNICATION_H
#include <Arduino.h>
#include <WiFi.h>

void spaCommunicationSetup();
void spaCommunicationLoop(IPAddress);
#endif