#include <Arduino.h>
#include <ESP8266mDNS.h>
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <FS.h>
#include "WebServerUtils.h"

void initAccessPoint(WiFiMode wifiMode);
String getContentType(String filename);

void initAccessPoint(WiFiMode wifiMode) {
    if (wifiMode == WIFI_AP) {
        WiFi.mode(WIFI_AP);
        WiFi.softAP(USERNAME, PASSWORD);
        Serial.print("IP address: ");
        Serial.println(WiFi.localIP());
        Serial.print("IP of access point's network interface: ");
        Serial.println(WiFi.softAPIP());
    }
    else if (wifiMode == WIFI_STA) {
        WiFi.mode(WIFI_STA);
        WiFi.begin(WIFI_NAME, WIFI_PASSWORD);
    
        while (WiFi.status() != WL_CONNECTED) {
                delay(500);
                Serial.print(".");
        }

        Serial.println("");
        Serial.print("Connected to: ");
        Serial.println(WIFI_NAME);
        Serial.print("IP address: ");
        Serial.println(WiFi.localIP());
    }
    else {
        Serial.println("EXCEPTION: Invalid WifiMode!");
    }

    if (MDNS.begin("esp8266")) {
        Serial.println("mDNS responder started");
    } else {
        Serial.println("Error setting up MDNS responder!");
    }
}

bool handleFileRead(ESP8266WebServer* server, String path) {
    // send the right file to the client (if it exists)
    Serial.println("handleFileRead: " + path);

    // If a folder is requested, send the index file
    if (path.endsWith("/")) 
        path += "index.html";
    else if (!path.endsWith(".html")) {
        path += ".html";
    }

    String contentType = getContentType(path);              // Get the MIME type
    if (SPIFFS.exists(path)) {                              // If the file exists
        File file = SPIFFS.open(path, "r");                 // Open it
        size_t sent = (*server).streamFile(file, contentType); // And send it to the client
        file.close();                                       // Then close the file again
        return true;
    }
    
    Serial.println("\tFile Not Found");
    return false;
}

String getContentType(String filename) { // convert the file extension to the MIME type
    if (filename.endsWith(".html")) return "text/html";
    else if (filename.endsWith(".css")) return "text/css";
    else if (filename.endsWith(".js")) return "application/javascript";
    else if (filename.endsWith(".ico")) return "image/x-icon";
    return "text/plain";
}

bool isAuthentified(ESP8266WebServer* server){
  if ((*server).hasHeader("Cookie")){
    String cookie = (*server).header("Cookie");
    if (cookie.indexOf("ESPSESSIONID=1") != -1) {
      Serial.println("Authentification Successful");
      return true;
    }
  }
  Serial.println("Authentification Failed");
  return false;
}

void redirectToLogin(ESP8266WebServer* server) {
    (*server).sendHeader("Location","/login");
    (*server).sendHeader("Cache-Control","no-cache");
    (*server).send(301);
}
