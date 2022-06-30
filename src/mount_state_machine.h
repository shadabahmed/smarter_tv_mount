#ifndef MOUNT_STATE_MACHINE_H_
#define MOUNT_STATE_MACHINE_H_

#include "mount_controller.h"
#include "remote.h"
#define MAX_UP_CURRENT 400
#define MAX_DOWN_CURRENT 200
#define MAX_LEFT_CURRENT 40
#define MAX_RIGHT_CURRENT 40
#define MAX_TICKS_WITH_OVER_CURRENT 5
#define ZERO_CURRENT 2
#define MAX_TICKS_WITH_ZERO_CURRENT 5
#define MIN_DIST_FROM_WALL 150
#define MIN_SOFT_DIST_FROM_WALL 250
#define SETUP_WAIT 2000
#define TICK 5
#define TICKS_TO_WAIT_AFTER_FAULT 200 / TICK

class MountStateMachine {
  public: enum Event { NONE, DOWN_PRESSED, UP_PRESSED, RIGHT_PRESSED,
        LEFT_PRESSED, FAULT_DETECTED, DOWN_REACHED, UP_REACHED,
        RIGHT_REACHED, LEFT_REACHED, TV_TURNED_ON, TV_TURNED_OFF };
  public: enum State { STOPPED, MOVING_DOWN, MOVING_UP, MOVING_RIGHT,
        MOVING_LEFT, AUTO_MOVING_DOWN, AUTO_MOVING_UP, FAULT };

  public:
    MountStateMachine();
    State getState() { return this->state; };
    Event getEvent();
    void setup();
    bool transitionState(Event);
    static const char * getStateString(State state) { return StateStrings[state]; };
    static const char * getEventString(Event event) { return EventStrings[event]; };
    void printInfo(Event);
  private:
    MountController* mountController;
    Remote* remote;
    State state;
    Event getTvEvent();
    Event getRemoteEvent();    
    Event getUpDownMotorEvent();
    Event getLeftRightMotorEvent();
    Event getUpperLimitEvent();  
    Event getLowerLimitEvent();
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
    bool canMoveDown() { return noFaultCheck(); };
    bool canMoveUp() { return noFaultCheck() && mountController->getDistanceFromWall() > MIN_DIST_FROM_WALL;}
    bool shouldSlowMoveUp() { return mountController->getDistanceFromWall() < MIN_SOFT_DIST_FROM_WALL;}
    bool canMoveLeft()  { return noFaultCheck() && mountController->getDistanceFromWall() > MIN_DIST_FROM_WALL; };
    bool canMoveRight()  { return noFaultCheck() && mountController->getDistanceFromWall() > MIN_DIST_FROM_WALL; };
    bool noFaultCheck() { return state != FAULT; }

    inline static char * EventStrings[] =
        { "NONE", "DOWN_PRESSED", "UP_PRESSED", "RIGHT_PRESSED",
          "LEFT_PRESSED","FAULT_DETECTED", "DOWN_REACHED","UP_REACHED",
          "RIGHT_REACHED", "LEFT_REACHED", "TV_TURNED_ON", "TV_TURNED_OFF" };
    inline static char * StateStrings[] =
        { "STOPPED", "MOVING_DOWN", "MOVING_UP", "MOVING_RIGHT",
          "MOVING_LEFT","AUTO_MOVING_DOWN", "AUTO_MOVING_UP","FAULT" };
};

#endif