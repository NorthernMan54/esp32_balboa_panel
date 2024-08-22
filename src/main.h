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

#endif