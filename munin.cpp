#include "AquariumMonitor.h"
#include "munin.h"

PseudoMuninPluginField fields_uptime[] = {
  {
    .name = "uptime",
    .fetch = [](PseudoMuninPluginField* f) { f->value = millis(); 
      return true;}
  },
};

PseudoMuninPluginField fields_wtempR[] = {
  {
    .name = "wtempR",
    .fetch = [](PseudoMuninPluginField* f) { 
      f->value = g_wtempR;
      return true;}
  },
};

PseudoMuninPluginField fields_wtemp[] = {
  {
    .name = "wtemp",
    .fetch = [](PseudoMuninPluginField* f) { 
      if(g_wtemp > 0) {
        f->value = g_wtemp;
      }else{
        return false;
      }
      return true;
    }
  },
};

PseudoMuninPluginField fields_rtemp[] = {
  {
    .name = "rtemp",
    .fetch = [](PseudoMuninPluginField* f) { 
      f->value = g_rtemp;
      return true;}
  },
};

PseudoMuninPluginField fields_rhumid[] = {
  {
    .name = "rhumid",
    .fetch = [](PseudoMuninPluginField* f) { 
      f->value = g_rhumid;
      return true;}
  },
};
    
// definitions of munin plugin
PseudoMuninPlugin plugins[] = {
  {
    .name = "uptime",
    .config = R"(graph_title Uptime
graph_category system
graph_args --base 1000 --lower-limit 0
graph_scale yes
graph_vlabel uptime in milliseconds
uptime.label uptime [ms]
uptime.draw AREA)",
    .fields = fields_uptime,
    .numberOfFields = 1,
  },
  {
    .name = "temp_R",
    .config = R"(graph_title water temperature thermistor R
graph_category sensors
graph_args --base 1000 --lower-limit 9500 --upper-limit 11500 -r
graph_scale no
graph_vlabel Ohm
wtempR.label Water Temperature)",
    .fields = fields_wtempR,
    .numberOfFields = 1,
  },
  {
    .name = "wtemp",
    .config = R"(graph_title water temperature
graph_category sensors
graph_scale no
graph_vlabel degC
wtemp.label Water Temperature)",
    .fields = fields_wtemp,
    .numberOfFields = 1,
  },
  {
    .name = "roomtemp",
    .config = R"(graph_title room temperature
graph_category sensors
graph_scale no
graph_vlabel degC
rtemp.label Room Temperature)",
    .fields = fields_rtemp,
    .numberOfFields = 1,
  },
  {
    .name = "roomhumid",
    .config = R"(graph_title room humidity
graph_category sensors
graph_scale no
graph_vlabel %
rhumid.label Room Humidity)",
    .fields = fields_rhumid,
    .numberOfFields = 1,
  },
};

// pseudo munin-node instance
PseudoMuninNode node = {
  .plugins = plugins,
  .numberOfPlugins = 5
};
