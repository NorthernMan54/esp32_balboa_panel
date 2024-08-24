#include "findSpa.h"
#include <WiFi.h>
#include <WiFiUdp.h>
#include <ArduinoLog.h>
#include "../../src/utilities.h"

#define BALBOA_UDP_DISCOVERY_PORT 30303
#define BROADCAST_TIMEOUT 20 // 20 seconds
#define BROADCAST_INTERVAL 10 // 10 seconds

WiFiUDP udp;

void findSpaSetup()
{
  // put your setup code here, to run once:
}

/* Discovery Protocol

The WiFi Module can be discovered on an network by sending a UDP broadcast packet ('Discovery: Who is out there?') on port 30303. The WiFi Module will respond with two ASCII-encoded lines of text, separated by CRLF. The first line is the hostname (BWGSPA, e.g.), and the second line is its MAC address. The OUI for Balboa Instruments is 00:15:27. The IP address of the returned packet is the IP address of the WiFi module.

*/

char packetBuffer[255];
char ReplyBuffer[] = "BWGSPA\r\n00-15-27-00-00-01\r\n";
// Status of Discovery
bool connected = false;
bool initialSend = false;

IPAddress spaIP[10];
u_int8_t spaIPCount = 0;

unsigned long lastDiscoveryBroadcast = 0;
unsigned long lastDiscovery = 0;

void findSpaLoop()
{
  if (!connected && (lastDiscoveryBroadcast + BROADCAST_TIMEOUT / 3 < uptime() || !initialSend))
  {
    udp.begin(BALBOA_UDP_DISCOVERY_PORT);
    udp.beginPacket(IPAddress(255, 255, 255, 255), BALBOA_UDP_DISCOVERY_PORT);
    const char *message = "Discovery: This is the panel, Who is out there?";
    udp.write((const uint8_t *)message, strlen(message));
    udp.endPacket();
    Log.verbose(F("[UDP]: sent discovery message %s" CR), message);
    lastDiscoveryBroadcast = uptime();
    initialSend = true;
  }

  int packetSize = udp.parsePacket();
  if (packetSize)
  {
    // receive incoming UDP packets
    udp.read(packetBuffer, 255);
    udp.beginPacket(udp.remoteIP(), udp.remotePort());
    // udp.write((const uint8_t *)ReplyBuffer, strlen(ReplyBuffer));
    udp.endPacket();
    Log.verbose(F("[UDP]: Available Spa Connection %p %d" CR), udp.remoteIP(), udp.remotePort());
    Log.verbose(F("[UDP]: Spa Reply '%s'" CR), packetBuffer);
    if (spaIPCount < 10 && !spaAlreadyDiscovered(udp.remoteIP()))
    {
      spaIP[spaIPCount] = udp.remoteIP();
      Log.notice(F("[UDP]: Added Spa %p" CR), spaIP[spaIPCount]);
      spaIPCount++;
    }
    connected = true;
  }
}

bool spaAlreadyDiscovered(IPAddress ip)
{
  for (int i = 0; i < spaIPCount; i++)
  {
    if (spaIP[i] == ip)
    {
      return true;
    }
  }
  return false;
}