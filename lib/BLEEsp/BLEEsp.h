#ifndef BLEESP_H
#define BLEESP_H

#include "Arduino.h"
#include "BLEDevice.h"
#include "BLEUtils.h"
#include "BLEServer.h"

#define CONTROL_SERVICE_UUID "0000ffe0-0000-1000-8000-00805f9b34fb"
#define CONTROL_CHARACTERISTIC_UUID "0000ffe1-0000-1000-8000-00805f9b34fb"


class BLEEsp {
  private:
    BLEServer *server;
    BLECharacteristic controlCharacteristic = BLECharacteristic(
      CONTROL_CHARACTERISTIC_UUID,
      BLECharacteristic::PROPERTY_READ |
      BLECharacteristic::PROPERTY_WRITE
    );

  public:
    void begin(const char *name);
    BLECharacteristic * getControlCharacteristic();
};

#endif