#include "spaMessage.h"
#include <ArduinoLog.h>
#include "../../src/utilities.h"
#include "../../src/main.h"
#include "balboa.h"

#define TwoBit(value, bit) (((value) >> (bit)) & 0x03)

//
RTC_NOINIT_ATTR SpaStatusData spaStatusData;
RTC_NOINIT_ATTR SpaConfigurationData spaConfigurationData;
RTC_NOINIT_ATTR SpaInformationData spaInformationData;
RTC_NOINIT_ATTR SpaSettings0x04Data spaSettings0x04Data;
RTC_NOINIT_ATTR SpaFilterSettingsData spaFilterSettingsData;
RTC_NOINIT_ATTR SpaPreferencesData spaPreferencesData;
RTC_NOINIT_ATTR WiFiModuleConfigurationData wiFiModuleConfigurationData;

// private functions
void parseStatusMessage(u_int8_t *, int);
void parseInformationResponse(u_int8_t *, int);
void parseConfigurationResponse(u_int8_t *, int);
void parseWiFiModuleConfigurationResponse(u_int8_t *, int);
void parsePreferencesResponse(u_int8_t *, int);
void configurationRequest();

void spaMessageSetup()
{
  // put your setup code here, to run once:
  if(spaStatusData.magicNumber != 0x12345678){
    Log.verbose(F("spaMessageSetup()" CR));
    spaStatusData = {};
    spaStatusData.magicNumber = 0x12345678;
  }

  if(spaConfigurationData.magicNumber != 0x12345678){
    spaConfigurationData = {};
    spaConfigurationData.magicNumber = 0x12345678;
  }

  if(spaInformationData.magicNumber != 0x12345678){
    spaInformationData = {};
    spaInformationData.magicNumber = 0x12345678;
  }

  if(spaSettings0x04Data.magicNumber != 0x12345678){
    spaSettings0x04Data = {};
    spaSettings0x04Data.magicNumber = 0x12345678;
  }

  if(spaFilterSettingsData.magicNumber != 0x12345678){
    spaFilterSettingsData = {};
    spaFilterSettingsData.magicNumber = 0x12345678;
  }

  if(spaPreferencesData.magicNumber != 0x12345678){
    spaPreferencesData = {};
    spaPreferencesData.magicNumber = 0x12345678;
  }

  if(wiFiModuleConfigurationData.magicNumber != 0x12345678){
    wiFiModuleConfigurationData = {};
    wiFiModuleConfigurationData.magicNumber = 0x12345678;
  }
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
      case Preferences_Type:
        parsePreferencesResponse(message.message, message.length);
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
    if (spaConfigurationData.lastUpdate == 0 && spaConfigurationData.lastRequest == 0)
    {
      Log.verbose(F("Requesting Inital Configuration" CR));
      configurationRequest();
    }
    //  Log.verbose(F("No messages in Read Queue" CR));
  }
}

/*


*/

void configurationRequest()
{
  unsigned char byte_array[50];
  int offset = 0;

  unsigned char config_request[] = CONFIGURATION_REQUEST;
  unsigned char settings_request[] = SETTINGS_0X04_REQUEST;
  unsigned char filter_settings_request[] = FILTER_SETTINGS_REQUEST;
  unsigned char information_request[] = INFORMATION_REQUEST;

  if (spaConfigurationData.lastUpdate == 0 && spaConfigurationData.lastRequest == 0)
  {
    append_request(byte_array, &offset, config_request, sizeof(config_request));
    spaConfigurationData.lastRequest = getTime();
  }
  if (spaSettings0x04Data.lastUpdate == 0 && spaSettings0x04Data.lastRequest == 0)
  {
    append_request(byte_array, &offset, settings_request, sizeof(settings_request));
    spaSettings0x04Data.lastRequest = getTime();
  }
  if (spaFilterSettingsData.lastUpdate == 0 && spaFilterSettingsData.lastRequest == 0)
  {
    append_request(byte_array, &offset, filter_settings_request, sizeof(filter_settings_request));
    spaFilterSettingsData.lastRequest = getTime();
  }
  if (spaInformationData.lastUpdate == 0 && spaInformationData.lastRequest == 0)
  {
    append_request(byte_array, &offset, information_request, sizeof(information_request));
    spaInformationData.lastRequest = getTime();
  }

  spaWriteQueueMessage messageToSend;
  messageToSend.length = offset;
  memcpy(messageToSend.message, byte_array, offset);
  if (xQueueSend(spaWriteQueue, &messageToSend, 0) != pdTRUE)
  {
    Log.error(F("SPA Write Queue full, dropped %s" CR), msgToString(messageToSend.message, messageToSend.length).c_str());
  }
  else
  {
    Log.verbose(F("Data added to Write Queue %s" CR), msgToString(messageToSend.message, messageToSend.length).c_str());
  }
}

/*

Preferences Response

A Preferences Response is sent by the Main Board after a client sends the appropriate Settings Request (using the same Channel as the request) or when a client sends a Set Preference Request (using the broadcast Channel).

Type Code: 0x26

Length: 23

Arguments:

Byte(s)	Name	Description/Values
0	??	0
1	Reminders	0=OFF, 1=OFF
2	??	0
3	Temperature Scale	0=1°F, 1=0.5°C
4	Clock Mode	0=12-hour, 1=24-hour
5	Cleanup Cycle	0=OFF, 1-8 (30 minute increments)
6	Dolphin Address	0=none, 1-7=address
7	??	0
8	M8 Artificial Intelligence	0=OFF, 1=ON
9-17	??	0


*/

void parsePreferencesResponse(u_int8_t *message, int length)
{
  spaPreferencesData.crc = message[message[1]];
  spaPreferencesData.lastUpdate = getTime();

  u_int8_t *hexArray = message + 5;

  spaPreferencesData.reminders = hexArray[1];
  spaPreferencesData.tempScale = hexArray[3];
  spaPreferencesData.clockMode = hexArray[4];
  spaPreferencesData.cleanupCycle = hexArray[5];
  spaPreferencesData.dolphinAddress = hexArray[6];
  spaPreferencesData.m8AI = hexArray[8];

  Log.verbose(F("Preferences Response: %s" CR), msgToString(hexArray, length - 5).c_str());
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
  wiFiModuleConfigurationData.lastUpdate = getTime();

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
  spaConfigurationData.lastUpdate = getTime();

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
  spaInformationData.lastUpdate = getTime();

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
  spaStatusData.lastUpdate = getTime();

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