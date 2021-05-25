#include<Wire.h>
#include<Ticker.h>
#include <ESP8266mDNS.h>
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <FS.h>

#include "Accelerometer.h"
#include "WebServerUtils.h"
#include "WebServerPages.h"

#define RATE 5
#define INTERVAL 100

Ticker ticker;
ESP8266WebServer server(80);
Accelerometer accelerometer = Accelerometer();

float ac_x_calibration = 0.0f;
float ac_y_calibration = 0.0f;
float ac_z_calibration = 0.0f;

void measureAccelerometer();

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

	const char * headerkeys[] = {"User-Agent","Cookie"} ;
	size_t headerkeyssize = sizeof(headerkeys) / sizeof(char*);
	server.collectHeaders(headerkeys, headerkeyssize );

	server.begin(); 
	Serial.println("Web server started");

	ac_x_calibration = accelerometer.calibrate(ACCEL_XOUT_H);
	ac_y_calibration = accelerometer.calibrate(ACCEL_YOUT_H);
	ac_z_calibration = accelerometer.calibrate(ACCEL_ZOUT_H);

	Serial.println("Accelerometer ready!");

	ticker.attach_ms(INTERVAL, measureAccelerometer);

}

void loop() {
	server.handleClient();
}

void measureAccelerometer() {

	static uint32_t count = 0;
	static float measurement_x = 0.0f;
	static float measurement_y = 0.0f;
	static float measurement_z = 0.0f;

	measurement_x += accelerometer.read_accel_x(ac_x_calibration) / RATE;
    measurement_y += accelerometer.read_accel_y(ac_y_calibration) / RATE;
    measurement_z += accelerometer.read_accel_z(ac_z_calibration) / RATE;
	
	if (count % RATE == 0)
    {
        Serial.print("Accelerometer x: ");
        Serial.print(measurement_x);
        Serial.println();

        Serial.print("Accelerometer y: ");
        Serial.print(measurement_y);
        Serial.println();

        Serial.print("Accelerometer z: ");
        Serial.print(measurement_z);
        Serial.println();

        measurement_x = 0.0f;
        measurement_y = 0.0f;
        measurement_z = 0.0f;
    }

	count = count + 1;
}
