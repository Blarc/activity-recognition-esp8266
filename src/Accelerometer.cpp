#include<Wire.h>
#include <Arduino.h>
#include "Accelerometer.h"

Accelerometer::Accelerometer() {
  this->calibrate(this->accel_x_calibration, ACCEL_XOUT_H);
  this->calibrate(this->accel_y_calibration, ACCEL_YOUT_H);
  this->calibrate(this->accel_z_calibration, ACCEL_ZOUT_H);
}

void Accelerometer::calibrate(float calibration_var, int address) {
    Serial.print("Calibrating... ");
    for (int i = 0; i < ACCEL_CALIBRATION_SAMPLES; i++)
    {
        calibration_var += read_accel_data(address, 0);
        delay(100);
    }

    calibration_var /= ACCEL_CALIBRATION_SAMPLES;
    delay(1000);
    Serial.println("done!");
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

  return ((table / ACCEL_DIVIDER) - calibration);
}

int32_t Accelerometer::read_accel_x() {
  return read_accel_data(ACCEL_XOUT_H, accel_x_calibration);
}

int32_t Accelerometer::read_accel_y() {
  return read_accel_data(ACCEL_YOUT_H, accel_y_calibration);
}

int32_t Accelerometer::read_accel_z() {
  return read_accel_data(ACCEL_ZOUT_H, accel_z_calibration);
}




