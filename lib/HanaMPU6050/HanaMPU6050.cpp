#include "HanaMPU6050.h"


float rateRoll, ratePitch, rateYaw;
float rateCalibrationRoll, rateCalibrationPitch, rateCalibrationYaw;
int rateCalibrationNumber;

float accX, accY, accZ;
float angleRoll, anglePitch, angleYaw;

float kalmanAngleRoll = 0,
  kalmanUncertaintyAngleRoll = 2*2;
float kalmanAnglePitch = 0,
  kalmanUncertaintyAnglePitch = 2*2;

float kalman1DOutput[] = {0, 0};
float rad2deg = 180 / PI;

float gyro_meassurement = MPU6050_MEASUREMENT_250_DEG;
int accel_measurement = MPU6050_MEASUREMENT_2_G;


void HanaMPU6050::setClock(int clock) {
  Wire.setClock(clock);
}

void HanaMPU6050::setAccelerometerRange(mpu6050_accel_range_t range) {
  accel_range = range;

  if (range == MPU6050_RANGE_16_G) {
    accel_measurement = MPU6050_MEASUREMENT_16_G;
  } else if (range == MPU6050_RANGE_4_G) {
    accel_measurement = MPU6050_MEASUREMENT_4_G;
  } else if (range == MPU6050_RANGE_8_G) {
    accel_measurement = MPU6050_MEASUREMENT_8_G;
  }
}

void HanaMPU6050::setGyroRange(mpu6050_gyro_range_t gyro) {
  gyro_range = gyro;
  
  if (gyro == MPU6050_RANGE_500_DEG) {
    gyro_meassurement = MPU6050_MEASUREMENT_500_DEG;
  } else if (gyro == MPU6050_RANGE_1000_DEG) {
    gyro_meassurement = MPU6050_MEASUREMENT_1000_DEG;
  } else if (gyro == MPU6050_RANGE_2000_DEG) {
    gyro_meassurement = MPU6050_MEASUREMENT_2000_DEG;
  }
}

void HanaMPU6050::getAngle(mpu6050_angle_t *angle) {
  angle->roll = kalmanAngleRoll;
  angle->pitch = kalmanAnglePitch;
}

void HanaMPU6050::getAcceleration(mpu6050_accel_t *acc) {
  acc->x = accX;
  acc->y = accY;
  acc->z = accZ;
}

void HanaMPU6050::getRate(mpu6050_rate_t *rate) {
  rate->roll = rateRoll;
  rate->pitch = ratePitch;
  rate->yaw = rateYaw;
}

void HanaMPU6050::getGyro(mpu6050_gyro_t *gyro) {
  gyro->roll = rateRoll;
  gyro->pitch = ratePitch;
  gyro->yaw = rateYaw;
}

void HanaMPU6050::begin(double *ts, uint8_t i2c_addr) {
  i2c_address = i2c_addr;
  this->Ts = ts;
  
  Wire.begin();
  delay(250);
  
  Wire.beginTransmission(i2c_address);
  Wire.write(0x6B);
  Wire.write(0x00);
  Wire.endTransmission();

  for (
    rateCalibrationNumber = 0;
    rateCalibrationNumber < 2000;
    rateCalibrationNumber++
  ) {
    mpuSignals();
    rateCalibrationRoll += rateRoll;
    rateCalibrationPitch += ratePitch;
    rateCalibrationYaw += rateYaw;
  }

  rateCalibrationRoll /= 2000;
  rateCalibrationPitch /= 2000;
  rateCalibrationYaw /= 2000;
}

void HanaMPU6050::loop() {
  // put your main code here, to run repeatedly:
  mpuSignals();
  rateRoll -= rateCalibrationRoll;
  ratePitch -= rateCalibrationPitch;
  rateYaw -= rateCalibrationYaw;

  kalmanCalculate(
    kalmanAngleRoll,
    kalmanUncertaintyAngleRoll,
    rateRoll,
    angleRoll
  );
  kalmanAngleRoll = kalman1DOutput[0];
  kalmanUncertaintyAngleRoll = kalman1DOutput[1];
  

  kalmanCalculate(
    kalmanAnglePitch,
    kalmanUncertaintyAnglePitch,
    ratePitch,
    anglePitch
  );
  kalmanAnglePitch = kalman1DOutput[0];
  kalmanUncertaintyAnglePitch = kalman1DOutput[1];
}


void HanaMPU6050::mpuSignals(void) {
  Wire.beginTransmission(0x68);
  Wire.write(0x1A);
  Wire.write(0x05);
  Wire.endTransmission();
  Wire.beginTransmission(0x68);
  Wire.write(0x1C);
  Wire.write(0x10);
  Wire.endTransmission();
  Wire.beginTransmission(0x68);
  Wire.write(0x3B);
  Wire.endTransmission();
  Wire.requestFrom(0x68, 6);
  int16_t accXLSB = Wire.read() << 8 | Wire.read();
  int16_t accYLSB = Wire.read() << 8 | Wire.read();
  int16_t accZLSB = Wire.read() << 8 | Wire.read();

  Wire.beginTransmission(0x68);
  Wire.write(0x1B);
  Wire.write(0x8);
  Wire.endTransmission();
  Wire.beginTransmission(0x68);
  Wire.write(0x43);
  Wire.endTransmission();
  Wire.requestFrom(0x68, 6);
  int16_t gyroX = Wire.read() << 8 | Wire.read();
  int16_t gyroY = Wire.read() << 8 | Wire.read();
  int16_t gyroZ = Wire.read() << 8 | Wire.read();

  rateRoll = (float) gyroX / 65.5;
  ratePitch = (float) gyroY / 65.5;
  rateYaw = (float) gyroZ / 65.5;

  accX = (float) accXLSB / 4096 - ACCX_CORRECTION;
  accY = (float) accYLSB / 4096 + ACCY_CORRECTION;
  accZ = (float) accZLSB / 4096 - ACCZ_CORRECTION;

  angleRoll = atan(accY / sqrt(accZ * accZ + accX * accX)) * rad2deg;
  anglePitch = - atan(accX / sqrt(accZ * accZ + accY * accY)) * rad2deg;
}

void HanaMPU6050::kalmanCalculate(
  float kalmanState,
  float kalmanUncertainty,
  float kalmanInput,
  float kalmanMeasurement
) {
  kalmanState = kalmanState + *Ts * kalmanInput;
  kalmanUncertainty = kalmanUncertainty + *Ts * *Ts * 4 * 4;
  
  float kalmanGain = kalmanUncertainty * 1 / (1 * kalmanUncertainty + 3 * 3);
  kalmanState = kalmanState + kalmanGain * (kalmanMeasurement - kalmanState);
  kalmanUncertainty = kalmanUncertainty * (1 - kalmanGain);

  kalman1DOutput[0] = kalmanState;
  kalman1DOutput[1] = kalmanUncertainty;
}
