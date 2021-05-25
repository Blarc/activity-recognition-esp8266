#ifndef ACCELEROMETER_H
#define ACCELEROMETER_H

#define I2C_ADD_MPU 104

#define ACCEL_XOUT_H 59
#define ACCEL_XOUT_L 60
#define ACCEL_YOUT_H 61
#define ACCEL_YOUT_L 62
#define ACCEL_ZOUT_H 63
#define ACCEL_ZOUT_L 64

#define ACCEL_DIVIDER 131.0f
#define ACCEL_CALIBRATION_SAMPLES 100

class Accelerometer {

  private:
    float accel_x_calibration = 0.0f;
    float accel_y_calibration = 0.0f;
    float accel_z_calibration = 0.0f;
    void calibrate(float calibration_var, int address);
    int32_t read_accel_data(int address, float calibration);

  public:
    Accelerometer();
    int32_t read_accel_x();
    int32_t read_accel_y();
    int32_t read_accel_z();
    
};

#endif