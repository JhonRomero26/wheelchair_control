#include "Wheelchair.h"
#include "Arduino.h"
#include "SabertoothSimplified.h"


SabertoothSimplified st;
int oldTime = 0;
char directionCommand[32] = WHEELCHAIR_MOVE_STOP;
char cmd[32];

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
  ble->readCommand(cmd, sizeof(cmd));

  String cmdTemp(cmd);
  if (cmdTemp.indexOf("=") > 0) {
    if (cmdTemp.startsWith("e")) {
      float data = cmdTemp.substring(cmdTemp.indexOf("=") + 1).toInt();
      this->engineCorection = (float) data / 100;
    }
    if (cmdTemp.startsWith("ac"))
      this->acceleration = cmdTemp.substring(cmdTemp.indexOf("=") + 1).toInt();
  }

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
    strcmp(cmd, WHEELCHAIR_MOVE_STOP) == 0
  ) {
    strcpy(directionCommand, cmd);
  }

  if (
    strcmp(cmd, WHEELCHAIR_MOVE_LEFT) == 0 ||
    strcmp(cmd, WHEELCHAIR_MOVE_LEFT2) == 0 ||
    strcmp(cmd, WHEELCHAIR_MOVE_LEFT3) == 0
  ) {
    moveLeft();
  } else if (
    strcmp(cmd, WHEELCHAIR_MOVE_RIGHT) == 0 ||
    strcmp(cmd, WHEELCHAIR_MOVE_RIGHT2) == 0 ||
    strcmp(cmd, WHEELCHAIR_MOVE_RIGHT3) == 0
  ) {
    moveRight();
  } else if (strcmp(cmd, WHEELCHAIR_ROTATE_LEFT) == 0) {
    rotateLeft();
  } else if (strcmp(cmd, WHEELCHAIR_ROTATE_RIGHT) == 0) {
    rotateRight();
  } else {
    movements();
  }
}

void Wheelchair::movements() {
  if (strcmp(directionCommand, WHEELCHAIR_MOVE_FORWARD) == 0) {
    if (millis() - oldTime > TIME_FOR_MOVE) {
      moveForward();
      oldTime = millis();
    }
  } else if (strcmp(directionCommand, WHEELCHAIR_MOVE_STOP) == 0) {
    if (millis() - oldTime > TIME_FOR_STOP) {
      stop();
      oldTime = millis();
    }
  } else if (strcmp(directionCommand, WHEELCHAIR_MOVE_BACKWARD) == 0) {
    if (millis() - oldTime > TIME_FOR_MOVE) {
      moveBackward();
      oldTime = millis();
    }
  }
}

void Wheelchair::moveForward() {
  const int maxSpeed = MAX_SPEED_MOTOR * this->speedPercent;
  if (this->leftEngineSpeed > maxSpeed) this->leftEngineSpeed -= this->acceleration;
  if (this->rightEngineSpeed > maxSpeed) this->rightEngineSpeed -= this->acceleration;
  if (this->leftEngineSpeed < maxSpeed) this->leftEngineSpeed += this->acceleration;
  if (this->rightEngineSpeed < maxSpeed) this->rightEngineSpeed += this->acceleration;

  st.motor(MOTOR_LEFT, this->leftEngineSpeed);
  st.motor(MOTOR_RIGHT, this->rightEngineSpeed * this->engineCorection);
}

void Wheelchair::moveLeft() {
  const int maxSpeed = MAX_SPEED_MOTOR * this->speedPercent * this->engineCorection;
  int limiterSpeed = maxSpeed * WHEELCHAIR_MOVE_LEFT_RIGHT_PERCENT;

  if (strcmp(cmd, WHEELCHAIR_MOVE_LEFT2) == 0) {
    limiterSpeed = maxSpeed * WHEELCHAIR_MOVE_LEFT_RIGHT_PERCENT2;
  } else if (strcmp(cmd, WHEELCHAIR_MOVE_LEFT3) == 0) {
    limiterSpeed = maxSpeed * WHEELCHAIR_MOVE_LEFT_RIGHT_PERCENT3;
  }

  const int rightSpeed = maxSpeed + limiterSpeed;
  const int leftSpeed = maxSpeed - limiterSpeed;

  if (this->leftEngineSpeed < leftSpeed) this->leftEngineSpeed += this->acceleration;
  if (this->leftEngineSpeed > leftSpeed) this->leftEngineSpeed -= this->acceleration;
  if (this->rightEngineSpeed > rightSpeed) this->rightEngineSpeed -= this->acceleration;
  if (this->rightEngineSpeed < rightSpeed) this->rightEngineSpeed += this->acceleration;
  
  st.motor(MOTOR_LEFT, this->leftEngineSpeed);
  st.motor(MOTOR_RIGHT, this->rightEngineSpeed * this->engineCorection);
}

void Wheelchair::moveRight() {
  const int maxSpeed = MAX_SPEED_MOTOR * this->speedPercent * this->engineCorection;
  int limiterSpeed = maxSpeed * WHEELCHAIR_MOVE_LEFT_RIGHT_PERCENT;

  if (strcmp(cmd, WHEELCHAIR_MOVE_RIGHT2) == 0) {
    limiterSpeed = maxSpeed * WHEELCHAIR_MOVE_LEFT_RIGHT_PERCENT2;
  } else if (strcmp(cmd, WHEELCHAIR_MOVE_RIGHT3) == 0) {
    limiterSpeed = maxSpeed * WHEELCHAIR_MOVE_LEFT_RIGHT_PERCENT3;
  }

  const int rightSpeed = maxSpeed - limiterSpeed;
  const int leftSpeed = maxSpeed + limiterSpeed;

  if (this->leftEngineSpeed < leftSpeed) this->leftEngineSpeed += this->acceleration;
  if (this->leftEngineSpeed > leftSpeed) this->leftEngineSpeed -= this->acceleration;
  if (this->rightEngineSpeed > rightSpeed) this->rightEngineSpeed -= this->acceleration;
  if (this->rightEngineSpeed < rightSpeed) this->rightEngineSpeed += this->acceleration;
  
  st.motor(MOTOR_LEFT, this->leftEngineSpeed);
  st.motor(MOTOR_RIGHT, this->rightEngineSpeed * this->engineCorection);
}

void Wheelchair::rotateLeft() {
  const int maxSpeed = 15;

  const int rightSpeed = maxSpeed;
  const int leftSpeed = -(maxSpeed + 5);

  int acc = this->acceleration + 2;
  if (acc < 4) acc = 4;

  if (this->leftEngineSpeed < leftSpeed) this->leftEngineSpeed += acc;
  if (this->leftEngineSpeed > leftSpeed) this->leftEngineSpeed -= acc;
  if (this->rightEngineSpeed > rightSpeed) this->rightEngineSpeed -= acc;
  if (this->rightEngineSpeed < rightSpeed) this->rightEngineSpeed += acc;
  
  st.motor(MOTOR_LEFT, this->leftEngineSpeed * this->engineCorection);
  st.motor(MOTOR_RIGHT, this->rightEngineSpeed);
}

void Wheelchair::rotateRight() {
  const int maxSpeed = 15;

  const int rightSpeed = -(maxSpeed + 5);
  const int leftSpeed = maxSpeed;

  int acc = this->acceleration + 2;
  if (acc < 4) acc = 4;

  if (this->leftEngineSpeed < leftSpeed) this->leftEngineSpeed += acc;
  if (this->leftEngineSpeed > leftSpeed) this->leftEngineSpeed -= acc;
  if (this->rightEngineSpeed > rightSpeed) this->rightEngineSpeed -= acc;
  if (this->rightEngineSpeed < rightSpeed) this->rightEngineSpeed += acc;
  
  st.motor(MOTOR_LEFT, this->leftEngineSpeed * this->engineCorection);
  st.motor(MOTOR_RIGHT, this->rightEngineSpeed);
}

void Wheelchair::moveBackward() {
  const int maxSpeed = -MAX_SPEED_MOTOR * this->speedPercent;
  if (this->leftEngineSpeed > maxSpeed) this->leftEngineSpeed -= this->acceleration;
  if (this->rightEngineSpeed > maxSpeed) this->rightEngineSpeed -= this->acceleration;
  if (this->leftEngineSpeed < maxSpeed) this->leftEngineSpeed += this->acceleration;
  if (this->rightEngineSpeed < maxSpeed) this->rightEngineSpeed += this->acceleration;

  st.motor(MOTOR_LEFT, this->leftEngineSpeed * this->engineCorection);
  st.motor(MOTOR_RIGHT, this->rightEngineSpeed);
}

void Wheelchair::stop() {
  if (this->leftEngineSpeed == 0 && this->rightEngineSpeed == 0) return;

  int acc = this->acceleration + 3;
  if (acc < 5) acc = 5;
  
  if (this->leftEngineSpeed > 0) this->leftEngineSpeed -= this->acceleration;
  if (this->rightEngineSpeed > 0) this->rightEngineSpeed -= this->acceleration;
  if (this->leftEngineSpeed < 0) this->leftEngineSpeed += this->acceleration;
  if (this->rightEngineSpeed < 0) this->rightEngineSpeed += this->acceleration;

  st.motor(MOTOR_RIGHT, this->rightEngineSpeed);
  st.motor(MOTOR_LEFT, this->leftEngineSpeed);
}

