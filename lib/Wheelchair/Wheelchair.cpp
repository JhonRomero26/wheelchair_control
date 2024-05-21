#include "Wheelchair.h"
#include "Arduino.h"
#include "Sabertooth.h"


Sabertooth st(128);
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
  st.setBaudRate(bauds);
  st.motor(MOTOR_LEFT, 0);
  st.motor(MOTOR_RIGHT, 0);
}

void Wheelchair::loop() {
  char cmd[32];
  ble->readCommand(cmd, sizeof(cmd));
  const int currentTime = millis();

  String cmdTemp(cmd);
  if (cmdTemp.indexOf("=") > 0) {
    if (cmdTemp.startsWith("ec"))
      this->engineCorection = cmdTemp.substring(cmdTemp.indexOf("=") + 1).toFloat();
    if (cmdTemp.startsWith("sc"))
      this->maxSpeedCorection = cmdTemp.substring(cmdTemp.indexOf("=") + 1).toFloat();
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
  } else if (strcmp(directionCommand, WHEELCHAIR_ROTATE_LEFT) == 0) {
    if (currentTime - oldTime > TIME_FOR_MOVE) {
      rotateLeft();
      oldTime = millis();
    }
  } else if (strcmp(directionCommand, WHEELCHAIR_ROTATE_RIGHT) == 0) {
    if (currentTime - oldTime > TIME_FOR_MOVE) {
      rotateRight();
      oldTime = millis();
    }
  }
}

void Wheelchair::moveForward() {
  const int maxSpeed = MAX_SPEED_MOTOR * this->speedPercent;
  if (this->leftEngineSpeed > maxSpeed) this->leftEngineSpeed -= 5;
  if (this->rightEngineSpeed > maxSpeed) this->rightEngineSpeed -= 5;
  if (this->leftEngineSpeed < maxSpeed) this->leftEngineSpeed += 5;
  if (this->rightEngineSpeed < maxSpeed) this->rightEngineSpeed += 5;

  st.motor(MOTOR_LEFT, this->leftEngineSpeed);
  st.motor(MOTOR_RIGHT, this->rightEngineSpeed * this->engineCorection);
}

void Wheelchair::moveLeft() {
  const int maxSpeed = MAX_SPEED_MOTOR * this->speedPercent * this->maxSpeedCorection;
  int limiterSpeed = maxSpeed * WHEELCHAIR_MOVE_LEFT_RIGHT_PERCENT;

  if (strcmp(directionCommand, WHEELCHAIR_MOVE_LEFT2) == 0) {
    limiterSpeed = maxSpeed * WHEELCHAIR_MOVE_LEFT_RIGHT_PERCENT2;
  } else if (strcmp(directionCommand, WHEELCHAIR_MOVE_LEFT3) == 0) {
    limiterSpeed = maxSpeed * WHEELCHAIR_MOVE_LEFT_RIGHT_PERCENT3;
  }

  const int rightSpeed = maxSpeed + limiterSpeed;
  const int leftSpeed = maxSpeed - limiterSpeed;

  if (this->leftEngineSpeed < leftSpeed) this->leftEngineSpeed += 5;
  if (this->leftEngineSpeed > leftSpeed) this->leftEngineSpeed -= 5;
  if (this->rightEngineSpeed > rightSpeed) this->rightEngineSpeed -= 5;
  if (this->rightEngineSpeed < rightSpeed) this->rightEngineSpeed += 5;
  
  st.motor(MOTOR_LEFT, this->leftEngineSpeed);
  st.motor(MOTOR_RIGHT, this->rightEngineSpeed * this->engineCorection);
}

void Wheelchair::moveRight() {
  const int maxSpeed = MAX_SPEED_MOTOR * this->speedPercent * this->maxSpeedCorection;
  int limiterSpeed = maxSpeed * WHEELCHAIR_MOVE_LEFT_RIGHT_PERCENT;

  if (strcmp(directionCommand, WHEELCHAIR_MOVE_RIGHT2) == 0) {
    limiterSpeed = maxSpeed * WHEELCHAIR_MOVE_LEFT_RIGHT_PERCENT2;
  } else if (strcmp(directionCommand, WHEELCHAIR_MOVE_RIGHT3) == 0) {
    limiterSpeed = maxSpeed * WHEELCHAIR_MOVE_LEFT_RIGHT_PERCENT3;
  }

  const int rightSpeed = maxSpeed - limiterSpeed;
  const int leftSpeed = maxSpeed + limiterSpeed;

  if (this->leftEngineSpeed < leftSpeed) this->leftEngineSpeed += 5;
  if (this->leftEngineSpeed > leftSpeed) this->leftEngineSpeed -= 5;
  if (this->rightEngineSpeed > rightSpeed) this->rightEngineSpeed -= 5;
  if (this->rightEngineSpeed < rightSpeed) this->rightEngineSpeed += 5;
  
  st.motor(MOTOR_LEFT, this->leftEngineSpeed);
  st.motor(MOTOR_RIGHT, this->rightEngineSpeed * (this->engineCorection - 0.18));
}

void Wheelchair::rotateLeft() {
  const int maxSpeed = 15;

  const int rightSpeed = maxSpeed;
  const int leftSpeed = -(maxSpeed + 5);

  if (this->leftEngineSpeed < leftSpeed) this->leftEngineSpeed += 7;
  if (this->leftEngineSpeed > leftSpeed) this->leftEngineSpeed -= 7;
  if (this->rightEngineSpeed > rightSpeed) this->rightEngineSpeed -= 7;
  if (this->rightEngineSpeed < rightSpeed) this->rightEngineSpeed += 7;
  
  st.motor(MOTOR_LEFT, this->leftEngineSpeed * this->engineCorection);
  st.motor(MOTOR_RIGHT, this->rightEngineSpeed);
}

void Wheelchair::rotateRight() {
  const int maxSpeed = 15;

  const int rightSpeed = -(maxSpeed + 5);
  const int leftSpeed = maxSpeed;

  if (this->leftEngineSpeed < leftSpeed) this->leftEngineSpeed += 7;
  if (this->leftEngineSpeed > leftSpeed) this->leftEngineSpeed -= 7;
  if (this->rightEngineSpeed > rightSpeed) this->rightEngineSpeed -= 7;
  if (this->rightEngineSpeed < rightSpeed) this->rightEngineSpeed += 7;
  
  st.motor(MOTOR_LEFT, this->leftEngineSpeed);
  st.motor(MOTOR_RIGHT, this->rightEngineSpeed * this->engineCorection);
}

void Wheelchair::moveBackward() {
  const int maxSpeed = -20;
  if (this->leftEngineSpeed > maxSpeed) this->leftEngineSpeed -= 5;
  if (this->rightEngineSpeed > maxSpeed) this->rightEngineSpeed -= 5;
  if (this->leftEngineSpeed < maxSpeed) this->leftEngineSpeed += 5;
  if (this->rightEngineSpeed < maxSpeed) this->rightEngineSpeed += 5;

  st.motor(MOTOR_LEFT, this->leftEngineSpeed * this->engineCorection);
  st.motor(MOTOR_RIGHT, this->rightEngineSpeed);
}

void Wheelchair::stop() {
  if (this->leftEngineSpeed == 0 && this->rightEngineSpeed == 0) return;
  
  if (this->leftEngineSpeed > 0) this->leftEngineSpeed -= 5;
  if (this->rightEngineSpeed > 0) this->rightEngineSpeed -= 5;
  if (this->leftEngineSpeed < 0) this->leftEngineSpeed += 5;
  if (this->rightEngineSpeed < 0) this->rightEngineSpeed += 5;

  st.motor(MOTOR_RIGHT, this->rightEngineSpeed);
  st.motor(MOTOR_LEFT, this->leftEngineSpeed);
}
