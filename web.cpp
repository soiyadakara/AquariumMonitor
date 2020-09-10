#include "web.h"
#include "AquariumMonitor.h"
#include "settings.h"

// Async Webserver
AsyncWebServer server(80);


// Replaces placeholder with LED state value
String HTMLprocessor(const String& var){
  char buf[32];
  String strbuf = "";
  if(var == "wtemp"){
    return String(g_wtemp);
  }else if(var == "rtemp"){
    return String(g_rtemp);
  }else if(var == "rhumid"){
    return String(g_rhumid);
  }else if(var == "wtempR"){
    return String(g_wtempR);
  }else if(var == "st_led"){
    strbuf = (st_led & ST_MASK_SCHEDULED) ? "forced " : "scheduled ";
    strbuf += (st_led & ST_MASK_ONOFF) ? "ON" : "OFF";
    return strbuf;
  }else if(var == "st_co2"){
    strbuf = (st_co2 & ST_MASK_SCHEDULED) ? "forced " : "scheduled ";
    strbuf += (st_co2 & ST_MASK_ONOFF) ? "ON" : "OFF";
    return strbuf;
  }else if(var == "st_bukubuku"){
    strbuf = (st_bukubuku & ST_MASK_SCHEDULED) ? "forced " : "scheduled ";
    strbuf += (st_bukubuku & ST_MASK_ONOFF) ? "ON" : "OFF";
    return strbuf;
  }else if(var == "st_coolingfan"){
    strbuf = (st_coolingfan & ST_MASK_SCHEDULED) ? "forced " : "controlled ";
    strbuf += (st_coolingfan & ST_MASK_ONOFF) ? "ON" : "OFF";
    return strbuf;
  }else if(var == "st_ac"){
    strbuf = (st_ac & ST_MASK_SCHEDULED) ? "forced " : "controlled ";
    strbuf += (st_ac & ST_MASK_ONOFF) ? "ON" : "OFF";
    return strbuf;
  }else if(var == "md_led_forcedonc"){
    return (st_led == 3) ? "checked" : "";
  }else if(var == "md_led_forcedoffc"){
    return (st_led == 2) ? "checked" : "";
  }else if(var == "md_led_scheduledc"){
    return (st_led & 2) ? "" : "checked";
  }else if(var == "tm_led_on"){
    sprintf(buf, "%02d:%02d:%02d",tm_led_on/3600,(tm_led_on%3600)/60,tm_led_on%60);
    return String(buf);
  }else if(var == "tm_led_off"){
    sprintf(buf, "%02d:%02d:%02d",tm_led_off/3600,(tm_led_off%3600)/60,tm_led_off%60);
    return String(buf);
  }else if(var == "md_co2_forcedonc"){
    return (st_co2 == ST_FORCEDON) ? "checked" : "";
  }else if(var == "md_co2_forcedoffc"){
    return (st_co2 == ST_FORCEDOFF) ? "checked" : "";
  }else if(var == "md_co2_scheduledc"){
    return (st_co2 & ST_MASK_SCHEDULED) ? "" : "checked";
  }else if(var == "tm_co2_on"){
    sprintf(buf, "%02d:%02d:%02d",tm_co2_on/3600,(tm_co2_on%3600)/60,tm_co2_on%60);
    return String(buf);
  }else if(var == "tm_co2_off"){
    sprintf(buf, "%02d:%02d:%02d",tm_co2_off/3600,(tm_co2_off%3600)/60,tm_co2_off%60);
    return String(buf);
  }else if(var == "md_bukubuku_forcedonc"){
    return (st_bukubuku == ST_FORCEDON) ? "checked" : "";
  }else if(var == "md_bukubuku_forcedoffc"){
    return (st_bukubuku == ST_FORCEDOFF) ? "checked" : "";
  }else if(var == "md_bukubuku_scheduledc"){
    return (st_bukubuku & ST_MASK_SCHEDULED) ? "" : "checked";
  }else if(var == "tm_bukubuku_on"){
    sprintf(buf, "%02d:%02d:%02d",tm_bukubuku_on/3600,(tm_bukubuku_on%3600)/60,tm_bukubuku_on%60);
    return String(buf);
  }else if(var == "tm_bukubuku_off"){
    sprintf(buf, "%02d:%02d:%02d",tm_bukubuku_off/3600,(tm_bukubuku_off%3600)/60,tm_bukubuku_off%60);
    return String(buf);
  }else if(var == "md_CoolingFAN_forcedonc"){
    return (st_coolingfan == ST_FORCEDON) ? "checked" : "";
  }else if(var == "md_CoolingFAN_forcedoffc"){
    return (st_coolingfan == ST_FORCEDOFF) ? "checked" : "";
  }else if(var == "md_CoolingFAN_scheduledc"){
    return (st_coolingfan & ST_MASK_SCHEDULED) ? "" : "checked";
  }else if(var == "md_ac_scheduledc"){
    return (st_ac & ST_MASK_SCHEDULED) ? "" : "checked";
  }else if(var == "CoolingFANThres_on"){
    return String(g_CoolingFANThres_on);
  }else if(var == "CoolingFANThres_off"){
    return String(g_CoolingFANThres_off);
  }else if(var == "acThres_on"){
    return String(g_acThres_on);
  }else if(var == "whenupdated"){
    return g_whenupdated;
  }
}

void beginwebserver(){
  // Web server
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SPIFFS, "/index.html", String(), false, HTMLprocessor);
  });
  server.on("/config", HTTP_POST, [](AsyncWebServerRequest *request){
    String res = "in";

    for(int i=0;i<request->params();i++){
      AsyncWebParameter* p = request->getParam(i);
      if(p->name() == "tm_led_on"){
        tm_led_on = p->value().toInt();
        res += ",ln";
      }else if(p->name() == "tm_led_off"){
        tm_led_off = p->value().toInt();
        res += ",lf";
      }else if(p->name() == "tm_co2_on"){
        tm_co2_on = p->value().toInt();
        res += ",cn";
      }else if(p->name() == "tm_co2_off"){
        tm_co2_off = p->value().toInt();
        res += ",cf";
      }else if(p->name() == "tm_bukubuku_on"){
        tm_bukubuku_on = p->value().toInt();
        res += ",bn";
      }else if(p->name() == "tm_bukubuku_off"){
        tm_bukubuku_off = p->value().toInt();
        res += ",bf";
      }else if(p->name() == "CoolingFANThres_on"){
        g_CoolingFANThres_on = p->value().toFloat();
        res += ",fn";
      }else if(p->name() == "CoolingFANThres_off"){
        g_CoolingFANThres_off = p->value().toFloat();
        res += ",ff";
      }else if(p->name() == "acThres_on"){
        g_acThres_on = p->value().toFloat();
        res += ",an";
      }else if(p->name() == "md_led"){
        st_led = p->value().toInt();
        res += ",ml";
      }else if(p->name() == "md_co2"){
        st_co2 = p->value().toInt();
        res += ",mc";
      }else if(p->name() == "md_bukubuku"){
        st_bukubuku = p->value().toInt();
        res += ",mb";
      }else if(p->name() == "md_CoolingFAN"){
        st_coolingfan = p->value().toInt();
        res += ",mf";
      }else if(p->name() == "md_ac"){
        char t = p->value().toInt();
        if(!(t == ST_SCHEDULEDOFF && st_ac == ST_SCHEDULEDON)){
          st_ac = t;
          res += ",ac";
        }
      }
    }
    
    request->send(200, "text/plain", "okk!"+res);
  });
  server.begin();
}
