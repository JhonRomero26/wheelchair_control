#include "BleController.h"
#include "Wheelchair.h"

#define TXBLE 3
#define RXBLE 4

const char *bleName = "Hanalab-wheelchair";
const int bauds = 9600;

BleController ble(TXBLE, RXBLE);
Wheelchair controller(&ble);
char cmd[32];

void setup() {
  Serial.begin(bauds);
  ble.begin(bauds, bleName);
  controller.begin(bauds);
}

void loop() {
  controller.loop();
}
