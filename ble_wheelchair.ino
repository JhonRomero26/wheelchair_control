#include "BleController.h"
#include "Wheelchair.h"

#define RXBLE 4
#define TXBLE 3

const char *bleName = "Hanalab-wheelchair";
const int bauds = 9600;

BleController ble(TXBLE, RXBLE);
Wheelchair controller(&ble);

void setup() {
  Serial.begin(bauds);
  ble.begin(bauds, bleName);
  controller.begin(bauds);
}

void loop() {
}
