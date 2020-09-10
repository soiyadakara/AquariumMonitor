#ifndef _WEB_H_
#define _WEB_H_

// Web server
#include <ESPAsyncTCP.h>
#include <ESPAsyncWebServer.h>

extern AsyncWebServer server;

void beginwebserver();

#endif
