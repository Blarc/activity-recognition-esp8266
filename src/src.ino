#include<Wire.h>
#include<Ticker.h>
#include <ESP8266mDNS.h>
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <FS.h>

#include "Accelerometer.h"
#include "WebServerUtils.h"
#include "WebServerPages.h"

#define USERNAME "admin"
#define PASSWORD "admin"

#define WIFI_NAME "sdvincencij"
#define WIFI_PASSWORD "uzivajnaspletu"

#define PIN_LED 2

ESP8266WebServer server(80);    

void setup() {
	Serial.begin(115200);
	Serial.println();

	Wire.begin(12, 14);
	Wire.setClock(100000);

	SPIFFS.begin();

	initAccessPoint(WIFI_STA);

	server.on("/login", [&]() { handleLogin(&server); });
	server.on("/", [&]() { handleRoot(&server); });

  	server.onNotFound([&]() {
		if (!isAuthentified(&server)) {
			redirectToLogin(&server);
			return;
		}

		if (!handleFileRead(&server, server.uri())) {
			// otherwise, respond with a 404 (Not Found) error
			server.send(404, "text/plain", "404: Not Found");
			return;
		}
	});

	// here the list of headers to be recorded
	const char * headerkeys[] = {"User-Agent","Cookie"} ;
	size_t headerkeyssize = sizeof(headerkeys) / sizeof(char*);
	// ask server to track these headers
	server.collectHeaders(headerkeys, headerkeyssize );

	server.begin(); 
	Serial.println("Web server started");

	// Accelerometer accel = Accelerometer();
	// Serial.print("X: ");
	// Serial.println(accel.read_accel_x());
	// Serial.print("Y: ");
	// Serial.println(accel.read_accel_y());
	// Serial.print("Z: ");
	// Serial.println(accel.read_accel_z());

	Serial.println("Ready!");

}

void loop() {
	server.handleClient();
}
