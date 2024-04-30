#ifndef BLECONTROLLER_H
#define BLECONTROLLER_H
#include "SoftwareSerial.h"

class BleController {
  private:
    const char *name;
    SoftwareSerial *serial;
  
  public:
    BleController(int TX, int RX);
    void begin(int bauds, const char *name);
    void sendCommand(const char *cmd);
    const char * readCommand();
};

#endif
