#include "findSpa.h"
#include <WiFi.h>
#include <WiFiUdp.h>
#include <ArduinoLog.h>

#define BALBOA_UDP_DISCOVERY_PORT 30303

WiFiUDP Discovery;

// Status of Discovery
bool listening = false;

void findSpaSetup()
{
  // put your setup code here, to run once:
}

/* Discovery Protocol

The WiFi Module can be discovered on an network by sending a UDP broadcast packet ('Discovery: Who is out there?') on port 30303. The WiFi Module will respond with two ASCII-encoded lines of text, separated by CRLF. The first line is the hostname (BWGSPA, e.g.), and the second line is its MAC address. The OUI for Balboa Instruments is 00:15:27. The IP address of the returned packet is the IP address of the WiFi module.

*/

char packetBuffer[255];  
char ReplyBuffer[] = "BWGSPA\r\n00-15-27-00-00-01\r\n"; 

void findSpaLoop()
{

  if (!listening)
  {
    Discovery.beginMulticast(IPAddress(239, 255, 255, 250), BALBOA_UDP_DISCOVERY_PORT);
    listening = true;
  }

  int packetSize = Discovery.parsePacket();
  if (packetSize)
  {
    // receive incoming UDP packets
    Discovery.read(packetBuffer, 255);
    Discovery.beginPacket(Discovery.remoteIP(), Discovery.remotePort());
    Discovery.write((const uint8_t *)ReplyBuffer, strlen(ReplyBuffer));
    Discovery.endPacket();
    Log.verbose(F("[Discovery]: Spa Connection %s" CR), Discovery.remoteIP(), Discovery.remotePort());
  }
}