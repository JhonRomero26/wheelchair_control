#include "BleController.h"

#define RXBLE 4
#define TXBLE 3

BleController ble(TXBLE, RXBLE);
const char *bleName = "Hanalab-wheelchair";
const int bauds = 9600;

void setup() {
  Serial.begin(bauds);
  ble.begin(bauds, bleName);
}

void loop() {
}
