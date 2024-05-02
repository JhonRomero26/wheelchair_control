#include "Wheelchair.h"
#include "Arduino.h"
#include "SabertoothSimplified.h"


#define MAX_SPEED_MOTOR 127  // max speed motor

#ifndef TIME_FOR_MOVE
  #define TIME_FOR_MOVE  300   // time for move in ms
#endif

#ifndef TIME_FOR_STOP
  #define TIME_FOR_STOP  150   // time for move in ms
#endif

SabertoothSimplified st;
int oldTime = 0;
char directionCommand[32];

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

void Wheelchair::loop() {
  const char cmd[32];
  ble->readCommand(cmd, sizeof(cmd));
  const int currentTime = millis();

  if (
    strcmp(cmd, WHEELCHAIR_MOVE_FORWARD) == 0 ||
    strcmp(cmd, WHEELCHAIR_MOVE_BACKWARD) == 0 ||
    strcmp(cmd, WHEELCHAIR_MOVE_STOP) == 0 ||
    strcmp(cmd, WHEELCHAIR_ROTATE_LEFT) == 0 ||
    strcmp(cmd, WHEELCHAIR_ROTATE_RIGHT) == 0 ||
    strcmp(cmd, WHEELCHAIR_ROTATE_LEFT_VOICE) == 0 ||
    strcmp(cmd, WHEELCHAIR_ROTATE_RIGHT_VOICE) == 0
  ) {
    strcpy(directionCommand, cmd);
  }


  if (strcmp(directionCommand, WHEELCHAIR_MOVE_FORWARD) == 0) {
    if (currentTime - oldTime > TIME_FOR_MOVE) {
      moveForward();
      oldTime = millis();
    }
  }
}

void Wheelchair::moveForward() {
  const int maxSpeed = MAX_SPEED_MOTOR * this->speedPercent;
  if (
    this->leftEngineSpeed > maxSpeed &&
    this->rightEngineSpeed > maxSpeed
  ) return;

  this->leftEngineSpeed += 1;
  this->rightEngineSpeed += 1;

  st.motor(MOTOR_LEFT, this->leftEngineSpeed);
  st.motor(MOTOR_RIGHT, this->rightEngineSpeed);
}

void Wheelchair::stop() {
  if (this->leftEngineSpeed == 0 && this->rightEngineSpeed == 0) return;
  
  if (this->leftEngineSpeed > 0) this->leftEngineSpeed -= 1;
  if (this->rightEngineSpeed > 0) this->rightEngineSpeed -= 1;
  if (this->leftEngineSpeed < 0) this->leftEngineSpeed += 1;
  if (this->rightEngineSpeed < 0) this->rightEngineSpeed += 1;

  st.motor(MOTOR_RIGHT, this->rightEngineSpeed);
  st.motor(MOTOR_LEFT, this->leftEngineSpeed);
}
