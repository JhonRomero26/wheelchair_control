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

// MOVEMENT_COMMAND
#define WHEELCHAIR_MOVE_FORWARD "mf\0\n"
#define WHEELCHAIR_MOVE_LEFT "ml\0\n"
#define WHEELCHAIR_MOVE_BACKWARD "mb\0\n"
#define WHEELCHAIR_MOVE_RIGHT "mr\0\n"
#define WHEELCHAIR_MOVE_STOP "ms\0\n"
#define WHEELCHAIR_ROTATE_LEFT "rl\0\n"
#define WHEELCHAIR_ROTATE_RIGHT "rr\0\n"
#define WHEELCHAIR_ROTATE_LEFT_VOICE "rlv\0\n"
#define WHEELCHAIR_ROTATE_RIGHT_VOICE "rrv\0\n"

// SPEED_COMMAND
#define WHEELCHAIR_SPEED_SLOW "sv\0\n"
#define WHEELCHAIR_SPEED_MEDIUM "nv\0\n"
#define WHEELCHAIR_SPEED_FAST "fv\0\n"

Wheelchair::Wheelchair(BleController *ble) {
  this->ble = ble;
}

void Wheelchair::begin(int bauds) {
  SabertoothTXPinSerial.begin(bauds);
  st.motor(MOTOR_LEFT, 0);
  st.motor(MOTOR_RIGHT, 0);
}
