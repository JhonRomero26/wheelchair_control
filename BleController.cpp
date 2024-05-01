#include "BleController.h"
#include "Arduino.h"


BleController::BleController(int TX, int RX) {
  serial = new SoftwareSerial(RX, TX);
}

void BleController::begin(
  int bauds,
  const char *name
) {
  serial->begin(bauds);

  sendCommand("AT");
  sendCommand("AT+ROLE0");
  sendCommand(strcat("AT+NAME", name));
  sendCommand("AT+START");

  serial->write("");
}


void BleController::sendCommand(const char *cmd) {
  serial->println(cmd);

  char reply[1000];
  int i = 0;

  while(serial->available()) {
    reply[i] = serial->read();
    i += 1;
  }

  reply[i] = '\0';
  Serial.println(reply);
}

String BleController::readCommand() {
  String message = "";

  while (serial->available()) {
    char character = serial->read();
    message += character;
  }

  return message;
}
