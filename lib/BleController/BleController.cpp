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
  String nameCmd();
  String fullName("AT+NAME" + *name);

  sendCommand("AT");
  sendCommand("AT+ROLE0");
  sendCommand(fullName.c_str());
  sendCommand("AT+START");

  serial->setTimeout(10);
  delay(5000);
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

void BleController::readCommand(char *buffer, size_t bufferSize) {
  size_t l = serial->readBytesUntil('\n', buffer, bufferSize - 1);
  if (buffer[l - 1] == '\r') l--;
  buffer[l] = '\0';
}
