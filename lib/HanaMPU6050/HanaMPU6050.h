#ifndef HANA_MPU6050_H
#define HANA_MPU6050_H

#include "Arduino.h"
#include "Wire.h"

#ifndef HANA_MPU6050_CLOCK
#define HANA_MPU6050_CLOCK 400000
#endif

#ifndef ACCX_CORRECTION
  #define ACCX_CORRECTION 0.04
#endif

#ifndef ACCY_CORRECTION
  #define ACCY_CORRECTION 0.02
#endif

#ifndef ACCZ_CORRECTION
  #define ACCZ_CORRECTION 0.06
#endif

float _rad2deg = 1 / (3.142 /* PI */ / 180);

using degree = float;

struct KalmanResponse {
  degree state;
  degree uncertainty;
};

struct Acceleration {
  float x;
  float y;
  float z;
};

struct Angle {
  degree roll;
  degree pitch;
};

struct Rate {
  degree roll;
  degree pitch;
  degree yaw;
};


class HanaMPU6050 {
  private:
    float accX;
    float accY;
    float accZ;

    float rateRoll, _rateCalibrationRoll;
    float ratePitch, _rateCalibrationPitch;
    float rateYaw, _rateCalibrationYaw;
    float _rateCalibrationNumber;

    float angleRoll;
    float anglePitch;

    float kalmanAngleRoll = 0;
    float kalmanAnglePitch = 0;

    float kalmanUncertaintyAngleRoll = 2*2; // 2^4
    float kalmanUncertaintyAnglePitch = 2*2; // 2^4

    KalmanResponse _kalmanCalculate(
      float kalmanState, float kalmanUncertainty, float kalmanInput, float kalmanMeasurement);
    void mpuSignals();

  public:
    void begin();
    void loop();
    Acceleration getAcceleration();
    Angle getAngle();
    Rate getRate();
};

#endif
