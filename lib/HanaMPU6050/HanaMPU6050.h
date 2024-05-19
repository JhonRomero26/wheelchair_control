#ifndef HANA_MPU6050_H
#define HANA_MPU6050_H

#include "Arduino.h"
#include "Wire.h"

#define MPU6050_CLOCK_DEFAULT 400000
#define MPU6050_I2CADDRESS_DEFAULT 0x68

#ifndef ACCX_CORRECTION
  #define ACCX_CORRECTION 0.04
#endif

#ifndef ACCY_CORRECTION
  #define ACCY_CORRECTION 0.02
#endif

#ifndef ACCZ_CORRECTION
  #define ACCZ_CORRECTION 0.07
#endif


typedef struct accel_t {
  float x;
  float y;
  float z;
} mpu6050_accel_t;

typedef struct rate_t {
  float roll;
  float pitch;
  float yaw;
} mpu6050_rate_t;

typedef struct gyro_t {
  float roll;
  float pitch;
  float yaw;
} mpu6050_gyro_t;

typedef struct angle_t {
  float roll;
  float pitch;
} mpu6050_angle_t;


/**
 * @brief Accelerometer range options
 *
 * Allowed values for `setAccelerometerRange`.
 */
typedef enum {
  MPU6050_RANGE_2_G = 0b00,  ///< +/- 2g (default value)
  MPU6050_RANGE_4_G = 0b01,  ///< +/- 4g
  MPU6050_RANGE_8_G = 0b10,  ///< +/- 8g
  MPU6050_RANGE_16_G = 0b11, ///< +/- 16g
} mpu6050_accel_range_t;

/**
 * @brief Gyroscope range options
 *
 * Allowed values for `setGyroRange`.
 */
typedef enum {
  MPU6050_RANGE_250_DEG,  ///< +/- 250 deg/s (default value)
  MPU6050_RANGE_500_DEG,  ///< +/- 500 deg/s
  MPU6050_RANGE_1000_DEG, ///< +/- 1000 deg/s
  MPU6050_RANGE_2000_DEG, ///< +/- 2000 deg/s
} mpu6050_gyro_range_t;


#define MPU6050_MEASUREMENT_2_G 16384 ///< +/- 2g (default value)
#define MPU6050_MEASUREMENT_4_G 8192  ///< +/- 4g
#define MPU6050_MEASUREMENT_8_G 4096  ///< +/- 8g
#define MPU6050_MEASUREMENT_16_G 2048 ///< +/- 16g

#define MPU6050_MEASUREMENT_250_DEG 131.0  ///< +/- 250 deg/s (default value)
#define MPU6050_MEASUREMENT_500_DEG 65.5  ///< +/- 500 deg/s
#define MPU6050_MEASUREMENT_1000_DEG 32.8 ///< +/- 1000 deg/s
#define MPU6050_MEASUREMENT_2000_DEG 16.4 ///< +/- 2000 deg/s

#define RAD_2_DEG 180 / PI

class HanaMPU6050 {
  private:
    double rateCalibrationRoll = 0;
    double rateCalibrationPitch = 0;
    double rateCalibrationYaw = 0;
    double rateCalibrationNumber = 0;

    double accAngleErrorRoll = 0;
    double accAngleErrorPitch = 0;

    double angleRoll = 0;
    double anglePitch = 0;

    double rateRoll = 0;
    double ratePitch = 0;
    double rateYaw = 0;

    double accX = 0;
    double accY = 0;
    double accZ = 0;

    double gyroRoll = 0;
    double gyroPitch = 0;
    double gyroYaw = 0;

    double kalmanUncertaintyAngleRoll = 2*2;
    double kalmanUncertaintyAnglePitch = 2*2;

    float gyro_meassurement = MPU6050_MEASUREMENT_250_DEG;
    int accel_measurement = MPU6050_MEASUREMENT_2_G;

    double *Ts;
    uint8_t i2c_address;
    mpu6050_accel_range_t accel_range = MPU6050_RANGE_2_G;
    mpu6050_gyro_range_t gyro_range = MPU6050_RANGE_250_DEG; 
    
    

    void kalmanCalculate(
      float kalmanState, float kalmanUncertainty, float kalmanInput, float kalmanMeasurement);
    void readAcceleration();
    void readRate();
    void mpuSignals();
    void calculateError();

  public:
    void begin(
      double *ts,
      uint8_t i2c_addr = MPU6050_I2CADDRESS_DEFAULT
    );
    void setClock(int clock = MPU6050_CLOCK_DEFAULT);
    void loop();

    void setAccelerometerRange(mpu6050_accel_range_t range);
    void setGyroRange(mpu6050_gyro_range_t range);

    void getRate(mpu6050_rate_t *rate);
    void getAcceleration(mpu6050_accel_t *acc);
    void getGyro(mpu6050_gyro_t *gyro);
    void getAngle(mpu6050_angle_t *angle);
};

#endif
