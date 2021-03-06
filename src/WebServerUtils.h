#ifndef WEB_SERVER_UTILS_H
#define WEB_SERVER_UTILS_H

#include <ESP8266mDNS.h>
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>

#define USERNAME "admin"
#define PASSWORD "admin"

#define WIFI_NAME "wifi_name"
#define WIFI_PASSWORD "wifi_password"


bool handleFileRead(ESP8266WebServer* server, String path);
void initAccessPoint(WiFiMode wifiMode);
bool isAuthentified(ESP8266WebServer* server);
void redirectToLogin(ESP8266WebServer* server);
void handleLogin(ESP8266WebServer* server);
String getContentType(String filename);

#endif