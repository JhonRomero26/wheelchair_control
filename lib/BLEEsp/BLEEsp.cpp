#include "BLEEsp.h"


void BLEEsp::begin(const char *name) {
  BLEDevice::init(name);
  BLEAdvertising *advertising = BLEDevice::getAdvertising();
  server = BLEDevice::createServer();

  // Services
  BLEService *controlServ = server->createService(CONTROL_SERVICE_UUID);
  controlServ->addCharacteristic(&controlCharacteristic);
  controlServ->start();

  // Advertising
  advertising->addServiceUUID(CONTROL_SERVICE_UUID);
  advertising->setScanResponse(true);
  advertising->setMinPreferred(0x06);
  advertising->setMinPreferred(0x12);
  BLEDevice::startAdvertising();
}

BLECharacteristic * BLEEsp::getControlCharacteristic() {
  return &controlCharacteristic;
}
