#include "HanaMPU6050.h"


void HanaMPU6050::begin() {
  Wire.setClock(HANA_MPU6050_CLOCK);
  Wire.begin();
  delay(250);
  Wire.beginTransmission(0x68);
  Wire.write(0x6B);
  Wire.write(0x00);
  Wire.endTransmission();

  for (
    _rateCalibrationNumber = 0;
    _rateCalibrationNumber < 2000;
    _rateCalibrationNumber++
  ) {
    mpuSignals();
    _rateCalibrationRoll += rateRoll;
    _rateCalibrationPitch += ratePitch;
    _rateCalibrationYaw += rateYaw;
  }

  _rateCalibrationRoll /= 2000;
  _rateCalibrationPitch /= 2000;
  _rateCalibrationYaw /= 2000;
}

void HanaMPU6050::loop() {
  mpuSignals();
  rateRoll -= _rateCalibrationRoll;
  ratePitch -= _rateCalibrationPitch;
  rateYaw -= _rateCalibrationYaw;

  KalmanResponse kalmanRoll = _kalmanCalculate(
    kalmanAngleRoll,
    kalmanUncertaintyAngleRoll,
    rateRoll,
    angleRoll
  );
  kalmanAngleRoll = kalmanRoll.state;
  kalmanUncertaintyAngleRoll = kalmanRoll.uncertainty;

  KalmanResponse kalmanPitch = _kalmanCalculate(
    kalmanAnglePitch,
    kalmanUncertaintyAnglePitch,
    ratePitch,
    anglePitch
  );
  kalmanAnglePitch = kalmanPitch.state;
  kalmanUncertaintyAnglePitch = kalmanPitch.uncertainty;
}

Acceleration HanaMPU6050::getAcceleration() {
  return {accX, accY, accZ};
}


Angle HanaMPU6050::getAngle() {
  return {angleRoll, anglePitch};
}


Rate HanaMPU6050::getRate() {
  return {rateRoll, ratePitch, rateYaw};
}

void HanaMPU6050::mpuSignals() {
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

  angleRoll = atan(accY / sqrt(accZ * accZ + accX * accX)) * _rad2deg;
  anglePitch = - atan(accX / sqrt(accZ * accZ + accY * accY)) * _rad2deg;
}

KalmanResponse HanaMPU6050::_kalmanCalculate(
  float kalmanState,
  float kalmanUncertainty,
  float kalmanInput,
  float kalmanMeasurement
) {
  kalmanState = kalmanState + 0.004 * kalmanInput;
  kalmanUncertainty = kalmanUncertainty + 0.004 * 0.004 * 4 * 4;
  
  float kalmanGain = kalmanUncertainty * 1 / (1 * kalmanUncertainty + 3 * 3);
  kalmanState = kalmanState + kalmanGain * (kalmanMeasurement - kalmanState);
  kalmanUncertainty = kalmanUncertainty * (1 - kalmanGain);

  return {kalmanState, kalmanUncertainty};
}