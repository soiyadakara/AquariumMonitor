#include "settings.h"

volatile int tm_led_on;
volatile int tm_led_off;
volatile int tm_co2_on;
volatile int tm_co2_off;
volatile int tm_bukubuku_on;
volatile int tm_bukubuku_off;
volatile unsigned char g_LEDFANSpeed;
volatile unsigned char g_CoolingFANSpeed;
volatile float g_CoolingFANThres_on;
volatile float g_CoolingFANThres_off;
volatile float g_acThres_on;
volatile float g_FANFreq;

// configfile
bool loadConfig() {
  File configFile = SPIFFS.open("/config.json", "r");
  if (!configFile) {
    return false;
  }

  size_t size = configFile.size();
  if (size > 1024) {
    return false;
  }

  // Allocate a buffer to store contents of the file.
  std::unique_ptr<char[]> buf(new char[size]);
  configFile.readBytes(buf.get(), size);

  StaticJsonDocument<200> doc;
  auto error = deserializeJson(doc, buf.get());
  if (error) {
    return false;
  }

  g_CoolingFANSpeed = doc["CoolingFANSpeed"].as<uint8_t>();
  g_LEDFANSpeed = doc["LEDFANSpeed"].as<uint8_t>();
  
  tm_bukubuku_on = doc["bukubuku_on"].as<int>();
  tm_bukubuku_off = doc["bukubuku_off"].as<int>();
  tm_led_on = doc["led_on"].as<int>();
  tm_led_off = doc["led_off"].as<int>();
  tm_co2_on = doc["co2_on"].as<int>();
  tm_co2_off = doc["co2_off"].as<int>();

  g_CoolingFANThres_on = doc["CoolingFANThres_on"].as<float>();
  g_CoolingFANThres_off = doc["CoolingFANThres_off"].as<float>();
  g_acThres_on = doc["acThres_on"].as<float>();
  g_FANFreq = doc["FANFreq"].as<int>();

  return true;
}

bool saveConfig() {
  StaticJsonDocument<200> doc;
  
  doc["CoolingFANSpeed"] = g_CoolingFANSpeed;
  doc["LEDFANSpeed"] = g_LEDFANSpeed;
  
  doc["bukubuku_on"] = tm_bukubuku_on;
  doc["bukubuku_off"] = tm_bukubuku_off;
  doc["led_on"] = tm_led_on;
  doc["led_off"] = tm_led_off;
  doc["co2_on"] = tm_co2_on;
  doc["co2_off"] = tm_co2_off;

  doc["CoolingFANThres_on"] = g_CoolingFANThres_on;
  doc["CoolingFANThres_off"] = g_CoolingFANThres_off;
  doc["acThres_on"] = g_acThres_on;
  doc["FANFreq"] = g_FANFreq;

  File configFile = SPIFFS.open("/config.json", "w");
  if (!configFile) {
    return false;
  }

  serializeJson(doc, configFile);
  return true;
}
