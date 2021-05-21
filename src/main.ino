#include<Wire.h>
#include<Ticker.h>
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include "Accelerometer.h"

#define USERNAME "admin"
#define PASSWORD "admin"

#define WIFI_NAME "sdvincencij"
#define WIFI_PASSWORD "uzivajnaspletu"

#define PIN_LED 2
#define WIFI_MODE 1

// Global variables
ESP8266WebServer server(80);

void setup() {
  Serial.begin(115200);
  Serial.println();

  Wire.begin(12, 14);
  Wire.setClock(100000);

  if (WIFI_MODE == 0) {
    WiFi.mode(WIFI_AP);
    WiFi.softAP(USERNAME, PASSWORD);
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());
    Serial.print("IP of access point's network interface: ");
    Serial.println(WiFi.softAPIP());
  }
  else if (WIFI_MODE == 1) {
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

  Accelerometer accel = Accelerometer();
  Serial.print("X: ");
  Serial.println(accel.read_accel_x());
  Serial.print("Y: ");
  Serial.println(accel.read_accel_y());
  Serial.print("Z: ");
  Serial.println(accel.read_accel_z());

  Serial.println("Ready!");

}

void loop() {
  server.handleClient();
}
