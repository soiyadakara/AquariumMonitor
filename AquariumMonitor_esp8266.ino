
#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <WiFiUdp.h>
#include <ArduinoOTA.h>
#include <Wire.h>
#include <TickerScheduler.h>
#include <interrupts.h>
#include "SSID.h"
// NTP
#include <time.h>
// ADC
#include <Adafruit_ADS1015.h>
// OLED
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
// DS18B20
#include <OneWire.h>
#include <DallasTemperature.h>
// HDC1000
#include <Adafruit_HDC1000.h>
// Remote control
#include <IRremoteESP8266.h>
#include <IRsend.h>
#include <ir_Hitachi.h>

#include "AquariumMonitor.h"
#include "munin.h"
#include "settings.h"
#include "web.h"

// global variables
volatile char st_bukubuku;
volatile char st_led;
volatile char st_co2;
volatile bool st_ledfan;
volatile char st_coolingfan;

volatile bool st_disp;
volatile char st_ac;

volatile float g_wtemp;
volatile float g_rtemp;
volatile float g_rhumid;
volatile float g_wtempR;
String g_whenupdated;

volatile int ac_whenon;

// Timer
TickerScheduler cronTicker(1);

// ADC
Adafruit_ADS1115 ads;  /* Use this for the 16-bit version */
#define ADS1115_Vcoef 0.125

// OLED
Adafruit_SSD1306 display(SSD1306_LCDWIDTH, SSD1306_LCDHEIGHT, &Wire);

// DS18B20
OneWire oneWire(PIN_1WIRE);
DallasTemperature watertemp(&oneWire);

// HDC1000
Adafruit_HDC1000 hdc;

// IR remote
IRHitachiAc424 ac(PIN_REMOCON);


void acpoweron(){
  uint8_t* rawdata;
  ac.on();
  ac.setFan(kHitachiAc424FanAuto);
  ac.setMode(kHitachiAc424Cool);
  ac.setTemp(26);
  ac.setButton(kHitachiAc424ButtonTempDown);
  rawdata = ac.getRaw();
  rawdata[17] = 0x80;
  rawdata[19] = 0x07;
  rawdata[23] = 0x10;     // set offtimer 2h
  ac.setRaw(rawdata);

  timer1_disable();
  ac.send();
  timer1_enable(TIM_DIV1, TIM_EDGE, TIM_SINGLE);
}

void cronfunc(){  
  time_t t;
  struct tm *tm;
  int hms;
  
  // get time
  t = time(NULL);
  tm = localtime(&t);
  hms = tm->tm_hour*3600+60*tm->tm_min+tm->tm_sec;

  // bukubuku
  if(!(st_bukubuku & ST_MASK_SCHEDULED)){ // if scheduled mode
    if(tm_bukubuku_on < tm_bukubuku_off){
      if(tm_bukubuku_on < hms && hms < tm_bukubuku_off){
        st_bukubuku = ST_SCHEDULEDON;
      }else{
        st_bukubuku = ST_SCHEDULEDOFF;
      }
    }else{
      if(tm_bukubuku_off < hms && hms < tm_bukubuku_on){
        st_bukubuku = ST_SCHEDULEDOFF;
      }else{
        st_bukubuku = ST_SCHEDULEDON;
      }
    }
  }

  // LED
  if(!(st_led & ST_MASK_SCHEDULED)){ // if scheduled mode
    if(tm_led_on < tm_led_off){
      if(tm_led_on < hms && hms < tm_led_off){
        st_led = ST_SCHEDULEDON;
      }else{
        st_led = ST_SCHEDULEDOFF;
      }
    }else{
      if(tm_led_off < hms && hms < tm_led_on){
        st_led = ST_SCHEDULEDOFF;
      }else{
        st_led = ST_SCHEDULEDON;
      }
    }
  }

  // CO2
  if(!(st_co2 & ST_MASK_SCHEDULED)){ // if scheduled mode
    if(tm_co2_on < tm_co2_off){
      if(tm_co2_on < hms && hms < tm_co2_off){
        st_co2 = ST_SCHEDULEDON;
      }else{
        st_co2 = ST_SCHEDULEDOFF;
      }
    }else{
      if(tm_co2_off < hms && hms < tm_co2_on){
        st_co2 = ST_SCHEDULEDOFF;
      }else{
        st_co2 = ST_SCHEDULEDON;
      }
    }
  }

  if(st_bukubuku & ST_MASK_ONOFF)
    digitalWrite(PIN_BUKUBUKU, HIGH);
  else
    digitalWrite(PIN_BUKUBUKU, LOW);

  if(st_led & ST_MASK_ONOFF){
    digitalWrite(PIN_LIGHT, HIGH);
    if(!st_ledfan){
      analogWriteFreq(g_FANFreq);
      analogWrite(PIN_LEDFAN, 255);
      delay(250);
      analogWrite(PIN_LEDFAN, g_LEDFANSpeed);
      st_ledfan = true;
    }
  }else{
    digitalWrite(PIN_LIGHT, LOW);
    analogWrite(PIN_LEDFAN, 0);
    st_ledfan = false;
  }

  if(st_co2 & ST_MASK_ONOFF)
    digitalWrite(PIN_CO2, HIGH);
  else
    digitalWrite(PIN_CO2, LOW);

  //read temps
  watertemp.begin();
  watertemp.requestTemperatures();
  g_wtemp = watertemp.getTempCByIndex(0);
  g_rtemp = hdc.readTemperature();
  g_rhumid = hdc.readHumidity();
  g_wtempR = 3.3*10000/(ADS1115_Vcoef*ads.readADC_SingleEnded(1)/1000)-10000;
  
  char buf[32];
  sprintf(buf, "%02d:%02d:%02d\n", tm->tm_hour, tm->tm_min, tm->tm_sec);
  g_whenupdated = String(buf);
  
  // check temp
  if(!(st_coolingfan & ST_MASK_SCHEDULED)){ // if scheduled mode
    if(!st_coolingfan && g_wtemp > g_CoolingFANThres_on){
      analogWrite(PIN_COOLINGFAN, 255);
      delay(250);
      analogWrite(PIN_COOLINGFAN, g_CoolingFANSpeed);
      st_coolingfan = ST_SCHEDULEDON;
    }else if(g_wtemp < g_CoolingFANThres_off){
      analogWrite(PIN_COOLINGFAN, 0);
      st_coolingfan = ST_SCHEDULEDOFF;
    }
  }else{
    if(st_coolingfan & ST_MASK_ONOFF){
      analogWrite(PIN_COOLINGFAN, 255);
      delay(250);
      analogWrite(PIN_COOLINGFAN, g_CoolingFANSpeed);
    }else{
      analogWrite(PIN_COOLINGFAN, 0);
    }
  }

  if(!(st_ac & ST_MASK_SCHEDULED)){
    if(st_ac == ST_SCHEDULEDOFF && g_wtemp > g_acThres_on){
      acpoweron();
      st_ac = ST_SCHEDULEDON;
    }
//    if(hms < 30*60){    // ACは一日一回、00:00 ~ 00:30の時にリセットする
    if(st_ac == ST_SCHEDULEDON && tm->tm_min == 0 && tm->tm_hour%2 == 0){  // 2時間に一度ACリセット判断
      st_ac = ST_SCHEDULEDOFF;
    }
  }else if(st_ac == ST_FORCEDON){
    acpoweron();
    st_ac = ST_FORCEDOFF;
  }
}

#ifndef STASSID
#warining SSID is not defined
#define STASSID "myssid"
#define STAPSK  "mypasswd"
#endif

const char* ssid = STASSID;
const char* password = STAPSK;

void setup() {
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  while (WiFi.waitForConnectResult() != WL_CONNECTED) {
    delay(5000);
    ESP.restart();
  }
  MDNS.begin("AquariumMonitor");
  ArduinoOTA.begin();

  // NTP
  configTzTime("JST-9", "ntp.nict.jp", "ntp.jst.mfeed.ad.jp");
//  configTime(  JST, 0, "ntp.nict.jp", "ntp.jst.mfeed.ad.jp");

  // SPIFFS
  SPIFFS.begin();
//  saveConfig();
  loadConfig();

  // GPIO
  pinMode(PIN_SW, INPUT);
  pinMode(PIN_LEDFAN, OUTPUT);
  pinMode(PIN_CO2, OUTPUT);
  digitalWrite(PIN_CO2, LOW);
  pinMode(PIN_COOLINGFAN, OUTPUT);
  pinMode(PIN_LIGHT, OUTPUT);
  digitalWrite(PIN_LIGHT, LOW);
  pinMode(PIN_BUKUBUKU, OUTPUT);
  digitalWrite(PIN_BUKUBUKU, LOW);
  pinMode(PIN_REMOCON, OUTPUT);

  analogWriteFreq(g_FANFreq);  // Freq 20kHz
  analogWriteRange(255);  // range 0~255
  analogWrite(PIN_LEDFAN, 0);
  analogWrite(PIN_COOLINGFAN, 0);

  Wire.begin(PIN_SDA, PIN_SCL);
  
  // Munin-node
  node.begin();

  // ADC                                                         ADS1015  ADS1115
  //                                                                -------  -------
  // ads.setGain(GAIN_TWOTHIRDS);  // 2/3x gain +/- 6.144V  1 bit = 3mV      0.1875mV (default)
  ads.setGain(GAIN_ONE);        // 1x gain   +/- 4.096V  1 bit = 2mV      0.125mV
  // ads.setGain(GAIN_TWO);        // 2x gain   +/- 2.048V  1 bit = 1mV      0.0625mV
  ads.begin();

  // initialize display
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);  // initialize with the I2C addr 0x3C (for the 128x32)
  display.clearDisplay();

  // text display tests
  display.setTextSize(2);
  display.setTextColor(WHITE);
  display.setCursor(0, 0);
  display.clearDisplay();
  display.println("AquaMon");
//  display.print("ver.");
  display.println(__DATE__);
  display.display();
  delay(2000);
  display.clearDisplay();
  display.display();

  // DS18B20
  watertemp.begin();
  watertemp.setResolution(11);  // Δ=0.25degC

  // HDC1000
  hdc.begin();
  
  // IRremote
  ac.begin();

  // Event
  st_bukubuku = ST_SCHEDULEDOFF;
  st_led = ST_SCHEDULEDOFF;
  st_co2 = ST_SCHEDULEDOFF;
  st_coolingfan = ST_SCHEDULEDOFF;
  st_ledfan = false;
  st_disp = false;
  st_ac = ST_FORCEDOFF;
  cronTicker.add(0, 60*1000, [&](void*) {cronfunc();}, nullptr, true);  // every 1 minutes

  // web server
  beginwebserver();
}

unsigned long prevmillis = 0;
void loop() {
  if(millis()-prevmillis > 1000){
    prevmillis = millis();
    if(digitalRead(PIN_SW) == LOW){
      st_disp = true;
      time_t t;
      struct tm *tm;

      t = time(NULL);
      tm = localtime(&t);
      display.clearDisplay();
      display.setCursor(0, 0);
      display.printf("%02d:%02d:%02d\n",
        tm->tm_hour, tm->tm_min, tm->tm_sec);

      display.print(String(g_wtemp,2));
      display.println(" degC");
      display.display();
      
//      acpoweron();
    }else if(st_disp && digitalRead(PIN_SW) == HIGH){
      st_disp = false;
      display.clearDisplay();
      display.display();
    }
//    if((st_bukubuku & ST_MASK_SCHEDULED) || (st_led & ST_MASK_SCHEDULED) || (st_co2 & ST_MASK_SCHEDULED) || (st_coolingfan & ST_MASK_SCHEDULED || st_acon)){
    if(st_ac == ST_FORCEDON){
      cronfunc();
    }
  }
  cronTicker.update();

  // wait a request from munin-update; respond commands and return values of field
  node.acceptClient();
  
  ArduinoOTA.handle();
}
