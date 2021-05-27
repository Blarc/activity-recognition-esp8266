#include <Arduino.h>
#include "WebServerUtils.h"
#include "WebServerPages.h"

void handleLogin(ESP8266WebServer* server){
    Serial.println("handleLogin called!");
    String msg;

    if ((*server).hasHeader("Cookie")){
        Serial.print("Found cookie: ");
        String cookie = (*server).header("Cookie");
        Serial.println(cookie);
    }

    if ((*server).hasArg("DISCONNECT")){
        Serial.println("Disconnect called!");
        (*server).sendHeader("Set-Cookie","ESPSESSIONID=0");
        redirectToLogin(server);
        return;
    }

    if ((*server).hasArg("USERNAME") && (*server).hasArg("PASSWORD")){
        if ((*server).arg("USERNAME") == USERNAME &&  (*server).arg("PASSWORD") == PASSWORD ){
            (*server).sendHeader("Location","/");
            (*server).sendHeader("Cache-Control","no-cache");
            (*server).sendHeader("Set-Cookie","ESPSESSIONID=1");
            (*server).send(301);
            Serial.println("Log in Successful");
            return;
        }
        msg = "Wrong username/password! try again.";
        Serial.println("Log in Failed");
    }
    
    handleFileRead(server, "/login.html");
}