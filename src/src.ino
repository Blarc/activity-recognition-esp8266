#include<Wire.h>
#include<Ticker.h>
#include <ESP8266mDNS.h>
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <ESP8266HTTPClient.h>
#include <FS.h>
#include <ArduinoJson.h>

#include "Accelerometer.h"
#include "WebServerUtils.h"

#define RATE 5
#define INTERVAL 10
#define N 10

Ticker ticker;
HTTPClient http;
ESP8266WebServer server(80);
Accelerometer accelerometer = Accelerometer();

String activityType;
bool sendData = false;
const size_t CAPACTIY = JSON_ARRAY_SIZE(N * 4);

float ac_x_calibration = 0.0f;
float ac_y_calibration = 0.0f;
float ac_z_calibration = 0.0f;

int32_t* data_x = (int32_t*) calloc(N, sizeof(int32_t));
int32_t* data_y = (int32_t*) calloc(N, sizeof(int32_t));
int32_t* data_z = (int32_t*) calloc(N, sizeof(int32_t));

float a_lf = 0.0158;
float b_lf = 0.9843;
float previous_lf_x = 0.0f;
float previous_lf_y = 0.0f;
float previous_lf_z = 0.0f;

void measureAccelerometer();

void setup() {
	Serial.begin(115200);
	Serial.println();

	Wire.begin(12, 14);
	Wire.setClock(100000);

	SPIFFS.begin();

	initAccessPoint(WIFI_STA);

	server.on("/login", [&]() { handleLogin(&server); });
	server.on("/record", [&]() {

		if (!isAuthentified(&server)){
			redirectToLogin(&server);
			return;
		}

		if (server.hasArg("ACTION")) {
			if (server.arg("ACTION") == "START") {
				Serial.println("Start recording called!");

				bool isConnected = http.begin("http://192.168.15.39:8080/data");
				Serial.print("Is connected to /data: ");
				Serial.println(isConnected);

				activityType = server.arg("TYPE");
				ticker.attach_ms(INTERVAL, measureAccelerometer);
			}
			else if (server.arg("ACTION") == "STOP") {
				Serial.println("Stop recording called!");
				ticker.detach();
			}
		}

		handleFileRead(&server, "/record.html");
	});

	server.on("/predict", [&]() {

		if (!isAuthentified(&server)){
			redirectToLogin(&server);
			return;
		}

		if (server.hasArg("ACTION")) {
			if (server.arg("ACTION") == "START") {
				Serial.println("Start predicting called!");

				bool isConnected = http.begin("http://192.168.15.39:8080/prediction");
				Serial.print("Is connected to /prediction: ");
				Serial.println(isConnected);

				ticker.attach_ms(INTERVAL, measureAccelerometer);
			}
			else if (server.arg("ACTION") == "STOP") {
				Serial.println("Stop predicting called!");
				ticker.detach();
			}
		}

		handleFileRead(&server, "/predict.html");
	});

	server.on("/", [&]() {

		if (!isAuthentified(&server)){
			redirectToLogin(&server);
			return;
		}

		handleFileRead(&server, "/root.html");
	});

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

	bool isConnected = http.begin("http://192.168.15.39:8080/data");
	Serial.print("Is connected to server: ");
	Serial.println(isConnected);

}

void loop() {
	server.handleClient();

	if (sendData) {
		http.addHeader("Content-Type", "application/json");
		Serial.println("Sending data... ");

		StaticJsonDocument<CAPACTIY> doc;
		JsonObject root = doc.to<JsonObject>();
		root["type"] = activityType;

		JsonArray array = root.createNestedArray("data");
		for (int i = 0; i < N; i++) {
			array.add(data_x[i]);
			array.add(data_y[i]);
			array.add(data_z[i]);
		}

		String json;
		serializeJson(doc, json);
		int httpResponseCode = http.POST(json);
		Serial.println(httpResponseCode);
		if (httpResponseCode == 200) {
			String prediction = http.getString();
			Serial.println(prediction);
		}
		sendData = false;
	}
}

void measureAccelerometer() {

	static uint32_t count = 0;
	static uint32_t index = 0;
	static int32_t measurement_x = 0;
	static int32_t measurement_y = 0;
	static int32_t measurement_z = 0;

	measurement_x += accelerometer.read_accel_x(ac_x_calibration);
    measurement_y += accelerometer.read_accel_y(ac_y_calibration);
    measurement_z += accelerometer.read_accel_z(ac_z_calibration);

	// low pass filter
	float current_lf_x = a_lf * measurement_x + b_lf * previous_lf_x;
	previous_lf_x = current_lf_x;

	float current_lf_y = a_lf * measurement_y + b_lf * previous_lf_y;
	previous_lf_y = current_lf_y;

	float current_lf_z = a_lf * measurement_z + b_lf * previous_lf_z;
	previous_lf_z = current_lf_z;

	measurement_x -= round(current_lf_x);
	measurement_y -= round(current_lf_y);
	measurement_z -= round(current_lf_z);

	if (index % N == 0) {
		sendData = true;
	}

	if (count % RATE == 0)
    {

		data_x[index % N] = measurement_x / RATE;
		data_y[index % N] = measurement_y / RATE;
		data_z[index % N] = measurement_z / RATE;

        measurement_x = 0.0f;
        measurement_y = 0.0f;
        measurement_z = 0.0f;

		
		index = index + 1;
    }

	count = count + 1;
}
