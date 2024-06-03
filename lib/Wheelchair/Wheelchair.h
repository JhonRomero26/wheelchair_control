#ifndef WHEELCHAIR_H
#define WHEELCHAIR_H

#include "BleController.h"

#define MAX_SPEED_MOTOR 127  // max speed motor

#ifndef TIME_FOR_MOVE
  #define TIME_FOR_MOVE  180   // time for move in ms
#endif

#ifndef TIME_FOR_STOP
  #define TIME_FOR_STOP  50   // time for move in ms
#endif

// MOVEMENT_COMMAND
#define WHEELCHAIR_MOVE_FORWARD "mf\0\n"
#define WHEELCHAIR_MOVE_LEFT "ml1\0\n"
#define WHEELCHAIR_MOVE_LEFT2 "ml2\0\n"
#define WHEELCHAIR_MOVE_LEFT3 "ml3\0\n"
#define WHEELCHAIR_MOVE_LEFT_OFF "ml0\0\n"
#define WHEELCHAIR_MOVE_BACKWARD "mb\0\n"
#define WHEELCHAIR_MOVE_RIGHT "mr1\0\n"
#define WHEELCHAIR_MOVE_RIGHT2 "mr2\0\n"
#define WHEELCHAIR_MOVE_RIGHT3 "mr3\0\n"
#define WHEELCHAIR_MOVE_RIGHT_OFF "mr0\0\n"
#define WHEELCHAIR_MOVE_STOP "ms\0\n"
#define WHEELCHAIR_ROTATE_LEFT "rl\0\n"
#define WHEELCHAIR_ROTATE_LEFT_OFF "rl0\0\n"
#define WHEELCHAIR_ROTATE_RIGHT "rr\0\n"
#define WHEELCHAIR_ROTATE_RIGHT_OFF "rr0\0\n"
#define WHEELCHAIR_ROTATE_LEFT_VOICE "rlv\0\n"
#define WHEELCHAIR_ROTATE_RIGHT_VOICE "rrv\0\n"

// SPEED_COMMAND
#define WHEELCHAIR_SPEED_SLOW "sv\0\n"
#define WHEELCHAIR_SPEED_MEDIUM "nv\0\n"
#define WHEELCHAIR_SPEED_FAST "fv\0\n"

#define WHEELCHAIR_SPEED_SLOW_VAlUE 0.12
#define WHEELCHAIR_SPEED_MEDIUM_VAlUE WHEELCHAIR_SPEED_SLOW_VAlUE + 0.07
#define WHEELCHAIR_SPEED_FAST_VAlUE WHEELCHAIR_SPEED_SLOW_VAlUE + 0.14

#define WHEELCHAIR_MOVE_LEFT_RIGHT_PERCENT 0.2
#define WHEELCHAIR_MOVE_LEFT_RIGHT_PERCENT2 WHEELCHAIR_MOVE_LEFT_RIGHT_PERCENT + 0.15
#define WHEELCHAIR_MOVE_LEFT_RIGHT_PERCENT3 WHEELCHAIR_MOVE_LEFT_RIGHT_PERCENT + 0.25

// CORRECTIONS COMMANDS
#define WHEELCHAIR_ENGINE_CORRECTION_PERCENT 0.85
#define WHEELCHAIR_MAX_SPEED_CORRECTION 0.78
#define WHEELCHAIR_ENGINE_CORRECTION_CMD "e"
#define WHEELCHAIR_SPEED_CORRECTION_CMD "ac"
#define WHEELCHAIR_VALUE_SEPARATOR_CMD "="


class Wheelchair {
  private:
    BleController *ble;
    int leftEngineSpeed = 0;
    int rightEngineSpeed = 0;
    int acceleration = 2;
    float speedPercent = WHEELCHAIR_SPEED_SLOW_VAlUE;
    int engineCorection = WHEELCHAIR_ENGINE_CORRECTION_PERCENT;
    int maxSpeedCorection = WHEELCHAIR_MAX_SPEED_CORRECTION;
  
  private:
    void moveForward();
    void stop();
    void moveBackward();
    void moveLeft();
    void moveRight();
    void rotateLeft();
    void rotateRight();
    void movements();

  public:
    Wheelchair(BleController *ble);
    void begin(int bauds);
    void loop();
};

#endif
