#ifndef PID_H
#define PID_H


// P(k) = kp * E(k)
// I(k) = ki * int_{0}^{k*Ts}(E(t) * dt)
// I(k) = I(k - 1) + ki * ( (E(k) + E(k-1)) * Ts ) / 2
// D(k) = kd * (E(k) + E(k-1)) / Ts


// C(k) = P(k) + I(k) + D(k)
// C(k) = kp * E(k) + I(k - 1) + ki * ( (E(k) + E(k-1)) * Ts ) / 2 + kd * (E(k) + E(k-1)) / Ts

// E(k) = desiredData - data

class PID {
  private:
    double kp = 0;
    double ki = 0;
    double kd = 0;
    double Ts = 0; // ms

    double maxLimit = 0;
    double minLimit = 0;
    
    double P = 0;
    double I = 0;
    double D = 0;
    
    double prevI = 0;
    double error = 0;
    double prevError = 0;

    double response = 0;

    void calcProportional();
    void calcIntegral();
    void calcDerivation();
    void calcError(double desired, double measure);

  public:
    PID(double kp, double ki, double kd, double Ts, double maxLimit, double minLimit);
    void calculate(double desired, double measure);
    void getResponse();
};

#endif
