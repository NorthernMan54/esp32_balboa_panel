#include "spaMessage.h"
#include <ArduinoLog.h>
#include "../../src/utilities.h"
#include "../../src/main.h"
#include "balboa.h"

#define TwoBit(value, bit) (((value) >> (bit)) & 0x03)

// private functions
void parseStatusMessage(u_int8_t *, int);
void parseInformationResponse(u_int8_t *, int);
void parseConfigurationResponse(u_int8_t *, int);
void parseWiFiModuleConfigurationResponse(u_int8_t *, int);

void spaMessageSetup()
{
  // put your setup code here, to run once:
}

void spaMessageLoop()
{
  if (uxQueueMessagesWaiting(spaReadQueue) > 0)
  {
    spaReadQueueMessage message;
    if (xQueueReceive(spaReadQueue, &message, 0) == pdTRUE)
    {
      switch (message.message[4])
      {
      case Status_Message_Type:
        parseStatusMessage(message.message, message.length);
        break;
      case Filter_Cycles_Type:
        Log.verbose(F("Filter Cycles Response: %s" CR), msgToString(message.message, message.length).c_str());
        break;
      case Information_Response_Type:
        parseInformationResponse(message.message, message.length);
        break;
      case Settings_0x04_Response_Type:
        Log.verbose(F("Settings 0x04 Response: %s" CR), msgToString(message.message, message.length).c_str());
        break;
      case Fault_Log_Type:
        Log.verbose(F("Fault Log Response: %s" CR), msgToString(message.message, message.length).c_str());
        break;
      case Configuration_Type:
        parseConfigurationResponse(message.message, message.length);
        break;
      case WiFi_Module_Configuration_Type:
        parseWiFiModuleConfigurationResponse(message.message, message.length);
        break;
      default:
        Log.verbose(F("Unknown Message Type: %x - %s" CR), message.message[4], msgToString(message.message, message.length).c_str());
      }
    }
  }
  else
  {
    if (spaConfigurationData.lastUpdate == 0 && spaConfigurationData.lastRequest + 60 * 60 * 12 < uptime())
    {
      Log.verbose(F("Requesting Configuration" CR));
      spaConfigurationData.lastRequest = uptime();
      // requestConfiguration();
    }
    //  Log.verbose(F("No messages in Read Queue" CR));
  }
}

/*

WiFi Module Configuration Response

A WiFi Module Configuration Response is sent by the WiFi Module when the App sends an Existing Client Request.

Type Code: 0x94

Length: 29

Arguments:

Byte(s)	Name	Values
0-2	??	??
3-8	Full MAC address	Varies
9-16	??	0
17-19	MAC address: OUI	00:15:27 (Balboa Instruments)
20-21	??	0xFF
22-24	MAC address: NIC-specific	Varies

*/

void parseWiFiModuleConfigurationResponse(u_int8_t *message, int length)
{
  wiFiModuleConfigurationData.crc = message[message[1]];
  wiFiModuleConfigurationData.lastUpdate = uptime();

  u_int8_t *hexArray = message + 5;

  sprintf(wiFiModuleConfigurationData.macAddress, "%02x:%02x:%02x:%02x:%02x:%02x", hexArray[3], hexArray[4], hexArray[5], hexArray[6], hexArray[7], hexArray[8]);

  Log.verbose(F("WiFi Module Configuration Response: %s" CR), msgToString(hexArray, length - 5).c_str());
}

/*

Configuration Response

A Configuration Response is sent by the Main Board after a client sends the appropriate Settings Request.

Type Code: 0x2E

Length: 11

Arguments:

Byte	Name	Values
0	Pumps 1-4	Bits N to N+1: Pump N/2+1 (0=None, 1=1-speed, 2=2-speed)
1	Pumps 5-6	Bits 0-1: Pump 5, Bits 6-7: Pump 6 (0=None, 1=1-speed, 2=2-speed)
2	Lights	Bits 0-1: Light 1, Bits 6-7: Light 2 (0=None, 1=Present)
3	Flags Byte 3	?Bits 0-1: Blower, Bit 7: Circulation Pump?
4	Flags Byte 4	?Bit 0: Aux 1, Bit 1: Aux 2, Bits 4-5: Mister?
5	??	0x00=??, 0x68=??



*/

void parseConfigurationResponse(u_int8_t *message, int length)
{
  spaConfigurationData.crc = message[message[1]];
  spaConfigurationData.lastUpdate = uptime();

  u_int8_t *hexArray = message + 5;

  spaConfigurationData.pump1 = TwoBit(hexArray[0], 0);
  spaConfigurationData.pump2 = TwoBit(hexArray[0], 2);
  spaConfigurationData.pump3 = TwoBit(hexArray[0], 4);
  spaConfigurationData.pump4 = TwoBit(hexArray[0], 6);

  spaConfigurationData.pump5 = TwoBit(hexArray[1], 0);
  spaConfigurationData.pump6 = TwoBit(hexArray[1], 2);

  spaConfigurationData.light1 = TwoBit(hexArray[2], 0);
  spaConfigurationData.light2 = TwoBit(hexArray[2], 2);

  spaConfigurationData.blower = TwoBit(hexArray[3], 0);
  spaConfigurationData.circulationPump = TwoBit(hexArray[3], 6);

  spaConfigurationData.aux1 = bitRead(hexArray[4], 0);
  spaConfigurationData.aux2 = bitRead(hexArray[4], 1);
  spaConfigurationData.mister = TwoBit(hexArray[4], 4);

  Log.verbose(F("Configuration Response: %s" CR), msgToString(hexArray, length - 5).c_str());
}

/*

Information Response

The Main Board sends a Filter Cycles Message when a client sends the appropriate Settings Request.

Type Code: 0x24

Length: 25

Arguments:

Byte(s)	Name	Description/Values
0-3	Software ID (SSID)	Displayed (in decimal) as "M<byte 0>_<byte 1> V<byte 2>[.<byte 3>]"
4-11	System Model Number	ASCII-encoded string
12	Current Configuration Setup Number	Refer to controller Tech Sheets
13-16	Configuration Signature	Checksum of the system configuration file
17	?Heater Voltage?	?0x01=240?
18	?Heater Type?	?0x06,0x0A=Standard?
19-20	DIP Switch Settings	LSB-first (bit 0 of Byte 19 is position 1)
*/

void parseInformationResponse(u_int8_t *message, int length)
{
  spaInformationData.crc = message[message[1]];
  spaInformationData.lastUpdate = uptime();

  u_int8_t *hexArray = message + 5;

  sprintf(spaInformationData.softwareID, "M%d_%d V%d.%d", hexArray[0], hexArray[1], hexArray[2], hexArray[3]);

  sprintf(spaInformationData.model, "%s%s%s%s%s%s%s%s", hexArray[4], hexArray[5], hexArray[6], hexArray[7], hexArray[8], hexArray[9], hexArray[10], hexArray[11]);

  spaInformationData.setupNumber = hexArray[12];
  spaInformationData.voltage = hexArray[17];
  spaInformationData.heaterType = hexArray[18];
  sprintf(spaInformationData.dipSwitch, "%x%x", hexArray[20], hexArray[19]);

  Log.verbose(F("Information Response: %s" CR), msgToString(hexArray, length - 5).c_str());
}

/*
Byte	Name	Description/Values
0	?Spa State?	0x00=Running, 0x01=Initializing, 0x05=Hold Mode, ?0x14=A/B Temps ON?, 0x17=Test Mode
1	?Initialization Mode?	0x00=Idle, 0x01=Priming Mode, 0x02=?Fault?, 0x03=Reminder, 0x04=?Stage 1?, 0x05=?Stage 3?, 0x42=?Stage 2?
2	Current Temperature	Temperature (scaled by Temperature Scale), 0xFF if unknown
3	Time: Hour	0-23
4	Time: Minute	0-59
5	Heating Mode	0=Ready, 1=Rest, 3=Ready-in-Rest
6	Reminder Type	0x00=None, 0x04=Clean filter, 0x0A=Check the pH, 0x09=Check the sanitizer, 0x1E=?Fault?
7	Sensor A Temperature / Hold Timer	Minutes if Hold Mode else Temperature (scaled by Temperature Scale) if A/B Temps else 0x01 if Test Mode else 0x00
8	Sensor B Temperature	Temperature (scaled by Temperature Scale) if A/B Temps else 0x00
9	Flags Byte 9	Temperature Scale, Clock Mode, Filter Mode (see below)
10	Flags Byte 10	Heating, Temperature Range (see below)
11	Flags Byte 11	Pumps 1-4 Status (see below)
12	Flags Byte 12	Pumps 5-6 Status (see below)
13	Flags Byte 13	Circulation Pump Status, Blower Status (see below)
14	Flags Byte 14	Lights 1-2 Status (see below)
15	Mister	0=OFF, 1=ON
16	??	0
17	??	0
18	Flags Byte 18	Notification Type (see below)
19	Flags Byte 19	Circulation Cycle, Notification (see below)
20	Set Temperature	Temperature (scaled by Temperature Scale)
21	Flags Byte 21	(see below)
22-23	??	0
24	M8 Cycle Time	0=OFF; 30, 60, 90, or 120 (in minutes)
25-26	??	0
*/

void parseStatusMessage(u_int8_t *message, int length)
{

  spaStatusData.crc = message[message[1]];
  spaStatusData.lastUpdate = uptime();

  u_int8_t *hexArray = message + 5;
  spaStatusData.spaState = hexArray[0];
  spaStatusData.initMode = hexArray[1];
  spaStatusData.currentTemp = (hexArray[2] != 0xff ? (hexArray[9] & 0x01 ? (float)hexArray[2] / 2 : hexArray[2]) : spaStatusData.currentTemp);
  // Combine hour and minute into a time string
  uint8_t hour = hexArray[3];
  uint8_t minute = hexArray[4];
  sprintf(spaStatusData.time, "%02d:%02d", hour, minute);

  spaStatusData.heatingMode = hexArray[5];
  spaStatusData.reminderType = hexArray[6];
  spaStatusData.sensorA = hexArray[7];
  spaStatusData.sensorB = hexArray[8];

  spaStatusData.tempScale = hexArray[9] & 0x01;
  spaStatusData.clockMode = hexArray[9] & 0x02;
  spaStatusData.filterMode = TwoBit(hexArray[9], 2);
  spaStatusData.panelLocked = hexArray[9] & 0x20;

  spaStatusData.tempRange = bitRead(hexArray[10], 2);
  spaStatusData.needsHeat = bitRead(hexArray[10], 3);
  spaStatusData.heatingState = TwoBit(hexArray[10], 4);

  spaStatusData.pump1 = TwoBit(hexArray[11], 0);
  spaStatusData.pump2 = TwoBit(hexArray[11], 2);
  spaStatusData.pump3 = TwoBit(hexArray[11], 4);
  spaStatusData.pump4 = TwoBit(hexArray[11], 6);
  spaStatusData.pump5 = TwoBit(hexArray[12], 0);
  spaStatusData.pump6 = TwoBit(hexArray[12], 2);

  spaStatusData.circ = bitRead(hexArray[13], 1);
  spaStatusData.blower = TwoBit(hexArray[13], 2);

  spaStatusData.light1 = bitRead(hexArray[14], 0);
  spaStatusData.light2 = bitRead(hexArray[14], 2);

  spaStatusData.mister = hexArray[15];

  spaStatusData.notification = hexArray[18];
  spaStatusData.flags19 = hexArray[19];

  spaStatusData.setTemp = hexArray[20];
  spaStatusData.settingsLock = bitRead(hexArray[21], 3);
  spaStatusData.m8CycleTime = hexArray[24];
}