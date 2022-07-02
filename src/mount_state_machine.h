#ifndef MOUNT_STATE_MACHINE_H_
#define MOUNT_STATE_MACHINE_H_

#include "mount_controller.h"
#include "remote.h"
#include "distance_sensors.h"

#define MAX_UP_CURRENT 375
#define MAX_DOWN_CURRENT 175
#define MAX_LEFT_CURRENT 200
#define MAX_RIGHT_CURRENT 200
#define MAX_TICKS_WITH_OVER_CURRENT 4
#define ZERO_CURRENT_VALUE 2
#define MAX_TICKS_WITH_ZERO_CURRENT 2
#define MIN_DIST_FROM_WALL 150
#define MIN_SOFT_DIST_FROM_WALL 250
#define SETUP_WAIT 2000
#define TICK 5
#define TICKS_TO_WAIT_AFTER_FAULT 200 / TICK
#define TICKS_TO_WAIT_FOR_TV_ON 3
#define MAX_SENSOR_DIFF 40
#define MIN_SENSOR_DIFF -40

class MountStateMachine {
  public: enum Event { NONE, DOWN_PRESSED, UP_PRESSED, RIGHT_PRESSED,
        LEFT_PRESSED, FAULT_DETECTED, BOTTOM_REACHED, TOP_REACHED,
        RIGHT_REACHED, LEFT_REACHED, TV_TURNED_ON, TV_TURNED_OFF };
  public: enum State { STOPPED, MOVING_DOWN, MOVING_UP, MOVING_RIGHT,
        MOVING_LEFT, AUTO_MOVING_DOWN, AUTO_MOVING_UP, FAULT };

  public:
    MountStateMachine();
    State getState() { return this->state; };
    Event getEvent();
    void begin();
    bool transitionState(Event);
    static const char * getStateString(State state) { return StateStrings[state]; };
    static const char * getEventString(Event event) { return EventStrings[event]; };
    void printInfo(Event);
  private:
    MountController* mountController;
    Remote* remote;
    DistanceSensors* sensors;
    State state;
    Event getTvEvent();
    Event getRemoteEvent();    
    Event getUpDownMotorFaultEvent();
    Event getLeftRightMotorFaultEvent();
    Event getUpperLimitEvent();  
    Event getLowerLimitEvent();
    Event getRightLimitEvent();
    Event getLeftLimitEvent();
    State getNextState(Event);
    State getNextStateForStopped(Event);
    State getNextStateForMovingDown(Event);
    State getNextStateForMovingUp(Event);
    State getNextStateForMovingRight(Event);
    State getNextStateForMovingLeft(Event);
    State getNextStateForAutoMovingUp(Event);
    State getNextStateForAutoMovingDown(Event);
    State getNextStateForFault(Event);
    bool transitionToStopped();
    bool transitionToMovingDown();
    bool transitionToMovingUp();
    bool transitionToMovingRight();
    bool transitionToMovingLeft();
    bool transitionToAutoMovingUp();
    bool transitionToAutoMovingDown();
    bool transitionToFault();
    bool canMoveDown() { return sensors->getMinDistance() < MAX_DISTANCE; };
    bool canMoveUp() { return wallDistanceCheck(); }
    bool shouldSlowMoveUp() { return sensors->getMinDistance() < MIN_SOFT_DIST_FROM_WALL;}
    bool canMoveLeft()  { return wallDistanceCheck() && sensors->getDistDiff() > MIN_SENSOR_DIFF; }
    bool canMoveRight()  { return wallDistanceCheck() && sensors->getDistDiff() < MAX_SENSOR_DIFF; }
    bool wallDistanceCheck() { return sensors->getMinDistance() > MIN_DIST_FROM_WALL; }

    inline static char * EventStrings[] =
        { "NONE", "DOWN_PRESSED", "UP_PRESSED", "RIGHT_PRESSED",
          "LEFT_PRESSED","FAULT_DETECTED", "BOTTOM_REACHED","TOP_REACHED",
          "RIGHT_REACHED", "LEFT_REACHED", "TV_TURNED_ON", "TV_TURNED_OFF" };
    inline static char * StateStrings[] =
        { "STOPPED", "MOVING_DOWN", "MOVING_UP", "MOVING_RIGHT",
          "MOVING_LEFT","AUTO_MOVING_DOWN", "AUTO_MOVING_UP","FAULT" };
};

#endif