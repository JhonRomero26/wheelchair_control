#include "HanaMPU6050.h"


float mpuPreviousTime, mpuCurrentTime;
float _kalman1DOutput[2] = {0, 0};

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
  angle->roll = angleRoll;
  angle->pitch = anglePitch;
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
  gyro->roll = gyroRoll;
  gyro->pitch = gyroPitch;
  gyro->yaw = gyroYaw;
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

  calculateError();

  mpuCurrentTime = micros();
}

void HanaMPU6050::loop() {
  // put your main code here, to run repeatedly:
  mpuSignals();
  double accAngleRoll = (atan(accY / sqrt(accZ * accZ + accX * accX)) * RAD_2_DEG) - accAngleErrorRoll;
  double accAnglePitch = (-atan(accX / sqrt(accZ * accZ + accY * accY)) * RAD_2_DEG) - accAngleErrorPitch;

  rateRoll -= rateCalibrationRoll;
  ratePitch -= rateCalibrationPitch;
  rateYaw -= rateCalibrationYaw;

  mpuPreviousTime = mpuCurrentTime;
  mpuCurrentTime = micros();
  float dt = (mpuCurrentTime - mpuPreviousTime) / 1000000;

  gyroRoll = 0.96 * (rateRoll * dt) + 0.04 * accAngleRoll;
  gyroPitch = 0.96 * (ratePitch * dt) + 0.04 * accAnglePitch;
  gyroYaw = rateYaw * dt;

  kalmanCalculate(
    angleRoll,
    kalmanUncertaintyAngleRoll,
    rateRoll,
    accAngleRoll
  );
  angleRoll = _kalman1DOutput[0];
  kalmanUncertaintyAngleRoll = _kalman1DOutput[1];
  

  kalmanCalculate(
    anglePitch,
    kalmanUncertaintyAnglePitch,
    ratePitch,
    accAnglePitch
  );
  anglePitch = _kalman1DOutput[0];
  kalmanUncertaintyAnglePitch = _kalman1DOutput[1];
}

void HanaMPU6050::readAcceleration() {
  Wire.beginTransmission(i2c_address);
  Wire.write(0x1A);
  Wire.write(0x05);
  Wire.endTransmission();
  Wire.beginTransmission(i2c_address);
  Wire.write(0x1C);
  Wire.write(accel_range);
  Wire.endTransmission();
  Wire.beginTransmission(i2c_address);
  Wire.write(0x3B);
  Wire.endTransmission();
  Wire.requestFrom(i2c_address, 6);

  int16_t accXLSB = Wire.read() << 8 | Wire.read();
  int16_t accYLSB = Wire.read() << 8 | Wire.read();
  int16_t accZLSB = Wire.read() << 8 | Wire.read();

  accX = (float) accXLSB / accel_measurement;
  accY = (float) accYLSB / accel_measurement;
  accZ = (float) accZLSB / accel_measurement;
}

void HanaMPU6050::readRate(void) {
  Wire.beginTransmission(i2c_address);
  Wire.write(0x1B);
  Wire.write(gyro_range);
  Wire.endTransmission();
  Wire.beginTransmission(i2c_address);
  Wire.write(0x43);
  Wire.endTransmission();
  Wire.requestFrom(i2c_address, 6);
  int16_t gyroX = Wire.read() << 8 | Wire.read();
  int16_t gyroY = Wire.read() << 8 | Wire.read();
  int16_t gyroZ = Wire.read() << 8 | Wire.read();

  rateRoll = (float) gyroX / gyro_meassurement;
  ratePitch = (float) gyroY / gyro_meassurement;
  rateYaw = (float) gyroZ / gyro_meassurement;
}

void HanaMPU6050::mpuSignals(void) {
  readAcceleration();
  readRate();
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

  _kalman1DOutput[0] = kalmanState;
  _kalman1DOutput[1] = kalmanUncertainty;
}

void HanaMPU6050::calculateError() {
  //When this function is called, ensure the car is stationary. See Step 2 for more info
  uint16_t maxCalibration = 2000;
  
  // Read accelerometer values maxCalibration times
  for (
    rateCalibrationNumber = 0;
    rateCalibrationNumber < maxCalibration;
    rateCalibrationNumber++
  ) {
    mpuSignals();
  
    accAngleErrorRoll += atan((accY) / sqrt(accX * accX + accZ * accZ)) * RAD_2_DEG;
    accAngleErrorPitch += - atan(accX / sqrt(accY * accY + accZ * accZ)) * RAD_2_DEG;

    rateCalibrationRoll += rateRoll;
    rateCalibrationPitch += ratePitch;
    rateCalibrationYaw += rateYaw;
  }

  //Divide the sum to get the error value
  accAngleErrorRoll /= maxCalibration;
  accAngleErrorPitch /= maxCalibration;

  //Divide the sum to get the error value
  rateCalibrationRoll /= maxCalibration;
  rateCalibrationPitch /= maxCalibration;
  rateCalibrationYaw /= maxCalibration;
  Serial.println("The the gryoscope setting in MPU6050 has been calibrated");
}