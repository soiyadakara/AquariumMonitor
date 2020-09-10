#ifndef _AQUARIUMMONITOR_H_
#define _AQUARIUMMONITOR_H_

#include <Arduino.h>

#define JST     3600*9

// PIN definition
#define PIN_SW          0
#define PIN_LEDFAN      13
#define PIN_CO2         12
#define PIN_COOLINGFAN  14
#define PIN_LIGHT       16
#define PIN_SCL         5
#define PIN_SDA         4
#define PIN_1WIRE       1
#define PIN_BUKUBUKU    3
#define PIN_REMOCON     15

#define ST_MASK_SCHEDULED 2
#define ST_MASK_ONOFF   1
#define ST_SCHEDULEDON  1
#define ST_SCHEDULEDOFF 0
#define ST_FORCEDON     3
#define ST_FORCEDOFF    2

extern volatile char st_bukubuku;
extern volatile char st_led;
extern volatile char st_co2;
extern volatile bool st_ledfan;
extern volatile char st_coolingfan;

extern volatile char st_ac;

extern volatile float g_wtemp;
extern volatile float g_rtemp;
extern volatile float g_rhumid;
extern volatile float g_wtempR;
extern String g_whenupdated;

#endif
