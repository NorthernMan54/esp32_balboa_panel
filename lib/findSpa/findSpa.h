#ifndef FINDSPA_H
#define FINDSPA_H

#include <Arduino.h>

void findSpaSetup();
void findSpaLoop();

bool spaAlreadyDiscovered(IPAddress ip);

#endif