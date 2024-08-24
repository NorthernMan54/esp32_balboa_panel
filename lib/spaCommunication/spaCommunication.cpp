#include "spaCommunication.h"
#include <ArduinoLog.h>
#include <WiFi.h>


void spaCommunicationSetup()
{
  Log.verbose(F("spaCommunicationSetup()" CR));
};

// SPA Communication State's

bool spaConnected = false;

void spaCommunicationLoop(IPAddress spaIP){
  if ( !spaConnected ) {
    Log.verbose(F("Connecting to Spa %p" CR), spaIP);
    spaConnected = true;
  } else {
    Log.verbose(F("Connected to Spa %p" CR), spaIP);
  }
};
