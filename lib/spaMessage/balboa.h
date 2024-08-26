#ifndef BALBOA_H
#define BALBOA_H
#include <Arduino.h>

// Message Types

#define New_Client_Clear_to_Send_Type 0x00
#define Channel_Assignment_Request_Type 0x01
#define Channel_Assignment_Response_Type 0x02
#define Channel_Assignment_Acknowledgement_Type 0x03
#define Existing_Client_Request_Type 0x04
#define Existing_Client_Response_Type 0x05
#define Clear_to_Send_Type 0x06
#define Nothing_to_Send_Type 0x07
#define Toggle_Item_Request_Type 0x11
#define Status_Message_Type 0x13
#define Set_Temperature_Type 0x20
#define Set_Time_Type 0x21
#define Settings_Request_Type 0x22
#define Filter_Cycles_Type 0x23
#define Information_Response_Type 0x24
#define Settings_0x04_Response_Type 0x25
#define Preferences_Type 0x26
#define Set_Preference_Request_Type 0x27
#define Fault_Log_Type 0x28
#define Lock_Settings_Type 0x2D
#define Configuration_Type 0x2E
#define WiFi_Module_Configuration_Type 0x94

// Configuration, Settings 0x04, Filter Cycles, and Information Messages
#define CONFIGURATION_REQUEST   {0x7e, 0x08, 0x0a, 0xbf, 0x22, 0x00, 0x00, 0x01, 0x58, 0x7e}
#define SETTINGS_0X04_REQUEST   {0x7e, 0x08, 0x0a, 0xbf, 0x22, 0x04, 0x00, 0x00, 0xf4, 0x7e}
#define FILTER_SETTINGS_REQUEST {0x7e, 0x08, 0x0a, 0xbf, 0x22, 0x01, 0x00, 0x00, 0x34, 0x7e}
#define INFORMATION_REQUEST     {0x7e, 0x08, 0x0a, 0xbf, 0x22, 0x02, 0x00, 0x00, 0x89, 0x7e}

struct SpaStatusData
{
  uint8_t crc;
  unsigned long lastUpdate = 0;

  uint8_t spaState;
  uint8_t initMode;
  float currentTemp;
  char time[6];
  uint8_t heatingMode;
  uint8_t reminderType;
  uint8_t sensorA;
  uint8_t sensorB;
  uint8_t tempScale;
  uint8_t clockMode;
  uint8_t filterMode;
  bool panelLocked;
  uint8_t tempRange;
  bool needsHeat;
  uint8_t heatingState;

  uint8_t pump1;
  uint8_t pump2;
  uint8_t pump3;
  uint8_t pump4;
  uint8_t pump5;
  uint8_t pump6;

  uint8_t circ;
  uint8_t blower;

  bool light1;
  bool light2;

  bool mister;

  float setTemp;
  uint8_t notification;
  uint8_t flags19;
  bool settingsLock;
  uint8_t m8CycleTime;
} spaStatusData;

struct SpaConfigurationData
{
  uint8_t crc;
  unsigned long lastUpdate = 0;
  unsigned long lastRequest = 0;

  uint8_t pump1;
  uint8_t pump2;
  uint8_t pump3;
  uint8_t pump4;
  uint8_t pump5;
  uint8_t pump6;
  uint8_t light1;
  uint8_t light2;
  bool circulationPump;
  bool blower;
  bool mister;
  bool aux1;
  bool aux2;
  bool temp_scale;
} spaConfigurationData;

struct SpaFilterSettingsData
{

  unsigned long lastUpdate = 0;
  unsigned long lastRequest = 0;

  uint8_t filt1Hour;
  uint8_t filt1Minute;
  uint8_t filt1DurationHour;
  uint8_t filt1DurationMinute;
  bool filt2Enable;
  uint8_t filt2Hour;
  uint8_t filt2Minute;
  uint8_t filt2DurationHour;
  uint8_t filt2DurationMinute;
} spaFilterSettingsData;

struct SpaFaultLogData
{
  uint8_t totEntry;
  uint8_t currEntry;
  uint8_t faultCode;
  String faultMessage;
  uint8_t daysAgo;
  uint8_t hour;
  uint8_t minutes;
} spaFaultLogData;

struct SpaInformationData
{
  uint8_t crc;
  unsigned long lastUpdate = 0;
  unsigned long lastRequest = 0;

  char softwareID[9];
  char model[9];
  uint8_t setupNumber;
  uint8_t voltage;
  uint8_t heaterType;
  char dipSwitch[3];
} spaInformationData;

struct WiFiModuleConfigurationData
{
  uint8_t crc;
  unsigned long lastUpdate = 0;
  unsigned long lastRequest = 0;

  char macAddress[11];
} wiFiModuleConfigurationData;

struct SpaSettings0x04Data
{
  uint8_t crc;
  unsigned long lastUpdate = 0;
  unsigned long lastRequest = 0;

} spaSettings0x04Data;

struct SpaPreferencesData
{
  uint8_t crc;
  unsigned long lastUpdate = 0;
  unsigned long lastRequest = 0;

  uint8_t reminders;
  uint8_t tempScale;
  uint8_t clockMode;
  uint8_t cleanupCycle;
  uint8_t dolphinAddress;
  uint8_t m8AI;
} spaPreferencesData;

#endif