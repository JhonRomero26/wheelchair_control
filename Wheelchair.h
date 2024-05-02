#ifndef WHEELCHAIR_H
#define WHEELCHAIR_H

#include "BleController.h"

class Wheelchair {
  private:
    BleController *ble;
    int leftEngineSpeed = 0;
    int rightEngineSpeed = 0;
    float speedPercent = 0.15;
  
  private:
    void moveForward();
    void stop();
    void moveBackward();
    void moveLeft();
    void moveRight();

  public:
    Wheelchair(BleController *ble);
    void begin(int bauds);
    void loop();
};

#endif
