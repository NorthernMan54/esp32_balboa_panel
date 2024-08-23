#ifndef MAIN_H
#define MAIN_H

#ifndef SERIAL_BAUD
#define SERIAL_BAUD 115200
#endif

#ifndef LOG_LEVEL
#define LOG_LEVEL LOG_LEVEL_VERBOSE
#endif

#define VERSION "0.0.1"
#define BUILD (String(__DATE__) + " - " + String(__TIME__)).c_str()

#define INITIAL_WDT_TIMEOUT 300 // Reset ESP32 if wifi is not connected within 5 minutes
#define RUNNING_WDT_TIMEOUT 60 // Reset ESP32 if no SPA messages are received for 60 seconds

#define logSection(section) Log.setShowLevel(false);Log.notice(F(CR "************* " section " **************" CR));Log.setShowLevel(true);

// Various status flags







#endif