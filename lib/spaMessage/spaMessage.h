#ifndef SPA_MESSAGE_H
#define SPA_MESSAGE_H
#include <Arduino.h>
#include "balboa.h"

extern RTC_NOINIT_ATTR SpaStatusData spaStatusData;
extern RTC_NOINIT_ATTR SpaConfigurationData spaConfigurationData;
extern RTC_NOINIT_ATTR SpaInformationData spaInformationData;
extern RTC_NOINIT_ATTR SpaSettings0x04Data spaSettings0x04Data;
extern RTC_NOINIT_ATTR SpaFilterSettingsData spaFilterSettingsData;
extern RTC_NOINIT_ATTR SpaPreferencesData spaPreferencesData;
extern RTC_NOINIT_ATTR WiFiModuleConfigurationData wiFiModuleConfigurationData;
extern RTC_NOINIT_ATTR SpaFaultLogData spaFaultLogData;

void spaMessageSetup();

void spaMessageLoop();

#endif