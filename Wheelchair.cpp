#include "Wheelchair.h"
#include "Arduino.h"
#include "SabertoothSimplified.h"


SabertoothSimplified st;
int oldTime = 0;
char directionCommand[32];

enum MOTORS {
  MOTOR_LEFT = 1,
  MOTOR_RIGHT = 2
};


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

  if (strcmp(cmd, WHEELCHAIR_SPEED_SLOW) == 0) {
    this->speedPercent = WHEELCHAIR_SPEED_SLOW_VAlUE;
  } else if (strcmp(cmd, WHEELCHAIR_SPEED_MEDIUM) == 0) {
    this->speedPercent = WHEELCHAIR_SPEED_MEDIUM_VAlUE;
  } else if (strcmp(cmd, WHEELCHAIR_SPEED_FAST) == 0) {
    this->speedPercent = WHEELCHAIR_SPEED_FAST_VAlUE;
  }

  if (
    strcmp(cmd, WHEELCHAIR_MOVE_FORWARD) == 0 ||
    strcmp(cmd, WHEELCHAIR_MOVE_BACKWARD) == 0 ||
    strcmp(cmd, WHEELCHAIR_MOVE_STOP) == 0 ||
    strcmp(cmd, WHEELCHAIR_ROTATE_LEFT) == 0 ||
    strcmp(cmd, WHEELCHAIR_ROTATE_RIGHT) == 0 ||
    strcmp(cmd, WHEELCHAIR_ROTATE_LEFT_VOICE) == 0 ||
    strcmp(cmd, WHEELCHAIR_ROTATE_RIGHT_VOICE) == 0 ||
    strcmp(cmd, WHEELCHAIR_MOVE_LEFT) == 0 ||
    strcmp(cmd, WHEELCHAIR_MOVE_LEFT2) == 0 ||
    strcmp(cmd, WHEELCHAIR_MOVE_LEFT3) == 0 ||
    strcmp(cmd, WHEELCHAIR_MOVE_RIGHT) == 0 ||
    strcmp(cmd, WHEELCHAIR_MOVE_RIGHT2) == 0 ||
    strcmp(cmd, WHEELCHAIR_MOVE_RIGHT3) == 0
  ) {
    strcpy(directionCommand, cmd);
  }


  if (strcmp(directionCommand, WHEELCHAIR_MOVE_FORWARD) == 0) {
    if (currentTime - oldTime > TIME_FOR_MOVE) {
      moveForward();
      oldTime = millis();
    }
  } else if (strcmp(directionCommand, WHEELCHAIR_MOVE_STOP) == 0) {
    if (currentTime - oldTime > TIME_FOR_STOP) {
      stop();
      oldTime = millis();
    }
  } else if (strcmp(directionCommand, WHEELCHAIR_MOVE_BACKWARD) == 0) {
    if (currentTime - oldTime > TIME_FOR_MOVE) {
      moveBackward();
      oldTime = millis();
    }
  } else if (
    strcmp(directionCommand, WHEELCHAIR_MOVE_LEFT) == 0 ||
    strcmp(directionCommand, WHEELCHAIR_MOVE_LEFT2) == 0 ||
    strcmp(directionCommand, WHEELCHAIR_MOVE_LEFT3) == 0
  ) {
    if (currentTime - oldTime > TIME_FOR_MOVE) {
      moveLeft();
      oldTime = millis();
    }
  } else if (
    strcmp(directionCommand, WHEELCHAIR_MOVE_RIGHT) == 0 ||
    strcmp(directionCommand, WHEELCHAIR_MOVE_RIGHT2) == 0 ||
    strcmp(directionCommand, WHEELCHAIR_MOVE_RIGHT3) == 0
  ) {
    if (currentTime - oldTime > TIME_FOR_MOVE) {
      moveRight();
      oldTime = millis();
    }
  }
}

void Wheelchair::moveForward() {
  const int maxSpeed = MAX_SPEED_MOTOR * this->speedPercent;
  if (this->leftEngineSpeed > maxSpeed) this->leftEngineSpeed -= 1;
  if (this->rightEngineSpeed > maxSpeed) this->rightEngineSpeed -= 1;
  if (this->leftEngineSpeed < maxSpeed) this->leftEngineSpeed += 1;
  if (this->rightEngineSpeed < maxSpeed) this->rightEngineSpeed += 1;

  st.motor(MOTOR_LEFT, this->leftEngineSpeed);
  st.motor(MOTOR_RIGHT, this->rightEngineSpeed * WHEELCHAIR_ENGINE_CORRECTION_PERCENT);
}

void Wheelchair::moveLeft() {
  const int maxSpeed = MAX_SPEED_MOTOR * this->speedPercent * WHEELCHAIR_MAX_SPEED_CORRECTION;
  int limiterSpeed = maxSpeed * WHEELCHAIR_MOVE_LEFT_RIGHT_PERCENT;

  if (strcmp(directionCommand, WHEELCHAIR_MOVE_LEFT2) == 0) {
    limiterSpeed = maxSpeed * WHEELCHAIR_MOVE_LEFT_RIGHT_PERCENT2;
  } else if (strcmp(directionCommand, WHEELCHAIR_MOVE_LEFT3) == 0) {
    limiterSpeed = maxSpeed * WHEELCHAIR_MOVE_LEFT_RIGHT_PERCENT3;
  }

  const int rightSpeed = maxSpeed + limiterSpeed;
  const int leftSpeed = maxSpeed - limiterSpeed;

  if (this->leftEngineSpeed < leftSpeed) this->leftEngineSpeed += 1;
  if (this->leftEngineSpeed > leftSpeed) this->leftEngineSpeed -= 1;
  if (this->rightEngineSpeed > rightSpeed) this->rightEngineSpeed -= 1;
  if (this->rightEngineSpeed < rightSpeed) this->rightEngineSpeed += 1;
  
  st.motor(MOTOR_LEFT, this->leftEngineSpeed);
  st.motor(MOTOR_RIGHT, this->rightEngineSpeed * WHEELCHAIR_ENGINE_CORRECTION_PERCENT);
}


void Wheelchair::moveRight() {
  const int maxSpeed = MAX_SPEED_MOTOR * this->speedPercent * WHEELCHAIR_MAX_SPEED_CORRECTION;
  int limiterSpeed = maxSpeed * WHEELCHAIR_MOVE_LEFT_RIGHT_PERCENT;

  if (strcmp(directionCommand, WHEELCHAIR_MOVE_RIGHT2) == 0) {
    limiterSpeed = maxSpeed * WHEELCHAIR_MOVE_LEFT_RIGHT_PERCENT2;
  } else if (strcmp(directionCommand, WHEELCHAIR_MOVE_RIGHT3) == 0) {
    limiterSpeed = maxSpeed * WHEELCHAIR_MOVE_LEFT_RIGHT_PERCENT3;
  }

  const int rightSpeed = maxSpeed - limiterSpeed;
  const int leftSpeed = maxSpeed + limiterSpeed;

  if (this->leftEngineSpeed < leftSpeed) this->leftEngineSpeed += 1;
  if (this->leftEngineSpeed > leftSpeed) this->leftEngineSpeed -= 1;
  if (this->rightEngineSpeed > rightSpeed) this->rightEngineSpeed -= 1;
  if (this->rightEngineSpeed < rightSpeed) this->rightEngineSpeed += 1;
  
  st.motor(MOTOR_LEFT, this->leftEngineSpeed);
  st.motor(MOTOR_RIGHT, this->rightEngineSpeed);
}


void Wheelchair::moveBackward() {
  const int maxSpeed = -20;
  if (this->leftEngineSpeed > maxSpeed) this->leftEngineSpeed -= 1;
  if (this->rightEngineSpeed > maxSpeed) this->rightEngineSpeed -= 1;
  if (this->leftEngineSpeed < maxSpeed) this->leftEngineSpeed += 1;
  if (this->rightEngineSpeed < maxSpeed) this->rightEngineSpeed += 1;

  st.motor(MOTOR_LEFT, this->leftEngineSpeed * WHEELCHAIR_ENGINE_CORRECTION_PERCENT);
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
