#ifndef WHEELCHAIR_H
#define WHEELCHAIR_H

#include "BleController.h"

class Wheelchair {
  private:
    BleController *ble;
  
  public:
    Wheelchair(BleController *ble);
    void begin(int bauds);
};

#endif
