# Activity recognition system with ESP8266

This is the implementation of an activity recognition system for ESP8266.

## Project structure

The repository contains the following folders:
- `backend` contains the server implementation in python for predicting the activity
- `src` contains the code for ESP8266 board

## Instructions

For running this project you need the ESP8266 board with MPU9250 sensor.

### ESP8266

For running the program on the board you need Arduino IDE and `ArduinoJson` library installed. In the `src/WebServerUtils.h` you need to change the WiFi credentials, optionally you can also change the username and password. You also need to change the `serverUrl` in `src/src.ino` to your computer's IP address.

### Server

To start the server, run the `backend.py`. The default server port is `8080`.

## Acknowledgements
Feature evaluation and feature extraction was done as described in the article *A Study on Human Activity Recognition Using Accelerometer Data from Smartphones* by Akram Bayat, Marc Pomplun, Duc A. Tran, which can be found [here](https://www.sciencedirect.com/science/article/pii/S1877050914008643).
