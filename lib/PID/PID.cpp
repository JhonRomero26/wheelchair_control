#include "PID.h"

PID::PID(
  double kp,
  double ki,
  double kd,
  double Ts,
  double maxLimit,
  double minLimit
) {
  this->kp = kp;
  this->ki = ki;
  this->kd = kd;
  this->Ts = Ts;

  this->maxLimit = maxLimit;
  this->minLimit = minLimit;
}


void PID::calcError(double desired, double measure) {
  this->prevError = this->error;
  this->error = desired - measure;
}

void PID::calcProportional() {
  this->P = this->kp * this->error;
}

void PID::calcIntegral() {
  this->prevI = this->I;
  this->I = this->prevI + this->ki * ((this->error + this->prevError) * this->Ts) / 2;
}

void PID::calcDerivation() {
  this->D = this->kd * (this->error + this->prevError) / this->Ts;
}


void PID::calculate(double desired, double measure) {
  calcError(desired, measure);
  calcProportional();
  calcIntegral();
  calcDerivation();

  this->response = this->P + this->I + this->D;

  if (this->response > this->maxLimit) this->response = maxLimit;
  if (this->response < this->minLimit) this->response = minLimit;
}
