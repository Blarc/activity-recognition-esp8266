#include<Wire.h>
#include <Arduino.h>
#include "Accelerometer.h"

float Accelerometer::calibrate(int address) {
	float calibration_var = 0.0f;
    for (int i = 0; i < ACCEL_CALIBRATION_SAMPLES; i++)
    {
    	calibration_var += read_accel_data(address, 0.0f);
        delay(100);
    }

   	calibration_var /= ACCEL_CALIBRATION_SAMPLES;
	return calibration_var;
}

int32_t Accelerometer::read_accel_data(int address, float calibration) {
  int32_t table;

  Wire.beginTransmission(I2C_ADD_MPU);
  Wire.write(address);
  Wire.endTransmission();

  Wire.requestFrom(I2C_ADD_MPU, 2);
  table = (int8_t)Wire.read();
  table = table << 8;
  table += (uint8_t)Wire.read();

  return ((table / ACCEL_DIVIDER) - (calibration));
}

int32_t Accelerometer::read_accel_x(float calibration) {
  	return read_accel_data(ACCEL_XOUT_H, calibration);
}

int32_t Accelerometer::read_accel_y(float calibration) {
  	return read_accel_data(ACCEL_YOUT_H, calibration);
}

int32_t Accelerometer::read_accel_z(float calibration) {
  	return read_accel_data(ACCEL_ZOUT_H, calibration);
}




