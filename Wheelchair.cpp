#include "Wheelchair.h"
#include "Arduino.h"
#include "SabertoothSimplified.h"

#define MAX_SPEED_MOTOR 127  // max speed motor

#ifndef TIME_FOR_MOVE
  #define TIME_FOR_MOVE  600   // time for move in ms
#endif

#ifndef TIME_FOR_STOP
  #define TIME_FOR_MOVE  150   // time for move in ms
#endif

SabertoothSimplified st;

enum MOTORS {
  MOTOR_LEFT = 1,
  MOTOR_RIGHT = 2
};

enum MOVEMENT_COMMAND {
  MOVE_FORWARD = 3,
  MOVE_LEFT = 4,
  MOVE_BACKWARD = 5,
  MOVE_RIGHT = 6,
  MOVE_STOP = 7,
  MOVE_CLOCKWISE = 8,
  MOVE_COUNTERCLOCKWISE = 9
};

enum SPEED_COMMAND {
  SPEED_SLOW = 0,
  SPEED_MEDIUM = 1,
  SPEED_FAST = 2
};

Wheelchair::Wheelchair(BleController *ble) {
  this->ble = ble;
}

void Wheelchair::begin(int bauds) {
  SabertoothTXPinSerial.begin(bauds);
  st.motor(MOTOR_LEFT, 0);
  st.motor(MOTOR_RIGHT, 0);
}
