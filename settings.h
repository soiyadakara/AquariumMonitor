#ifndef _STTINGS_H_
#define _SETTINGS_H_

// Filesystem
#include <FS.h>
#include <ArduinoJson.h>

extern volatile int tm_led_on;
extern volatile int tm_led_off;
extern volatile int tm_co2_on;
extern volatile int tm_co2_off;
extern volatile int tm_bukubuku_on;
extern volatile int tm_bukubuku_off;
extern volatile unsigned char g_LEDFANSpeed;
extern volatile unsigned char g_CoolingFANSpeed;
extern volatile float g_CoolingFANThres_on;
extern volatile float g_CoolingFANThres_off;
extern volatile float g_acThres_on;
extern volatile float g_FANFreq;

bool loadConfig();
bool saveConfig();

#endif
