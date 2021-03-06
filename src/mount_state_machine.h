#ifndef MOUNT_STATE_MACHINE_H_
#define MOUNT_STATE_MACHINE_H_

#include "mount_controller.h"
#include "remote.h"
#include "distance_sensors.h"

// TV CONFIG
#define TV_PIN 17
#define TV_ON 0
#define TV_CHANGE_DEBOUNCE_DURATION 100

// MOTOR CURRENTS
#define MAX_UP_CURRENT 400
#define MAX_DOWN_CURRENT 175
#define MAX_LEFT_CURRENT 200
#define MAX_RIGHT_CURRENT 200
#define ZERO_CURRENT_VALUE 2
#define MAX_DURATION_WITH_OVER_CURRENT 500
#define MAX_DURATION_WITH_ZERO_CURRENT 500
#define FAULT_WAIT_DURATION 2000
#define FAST_UP_DOWN_DUTY_CYCLE 50
#define SLOW_UP_DOWN_DUTY_CYCLE 25
#define LEFT_RIGHT_DUTY_CYCLE 100

// DISTANCE SENSOR CONFIG
#define MIN_DIST_FROM_WALL 90
#define MIN_SOFT_DIST_FROM_WALL 300
#define MAX_SENSOR_DIFF 100
#define MIN_SENSOR_DIFF -100

class MountStateMachine {
  public: enum Event { NONE, DOWN_PRESSED, UP_PRESSED, RIGHT_PRESSED,
        LEFT_PRESSED, FAULT_DETECTED, BOTTOM_REACHED, TOP_REACHED,
        RIGHT_REACHED, LEFT_REACHED, TV_TURNED_ON, TV_TURNED_OFF };
  public: enum State { STOPPED, MOVING_DOWN, MOVING_UP, MOVING_RIGHT,
        MOVING_LEFT, AUTO_MOVING_DOWN, AUTO_MOVING_UP, FAULT };

  public:
    MountStateMachine();
    void begin();
    void update();
  private:
    static const char * getStateString(State state) { return StateStrings[state]; };
    static const char * getEventString(Event event) { return EventStrings[event]; };
    MountController* mountController;
    Remote* remote;
    DistanceSensors* sensors;
    State state;
    unsigned long faultClearTimestamp;
    void refresh();
    void printInfo(Event);
    Event getEvent();
    bool transitionState(Event);
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
    State getNextStateForFault(Event) const;
    bool transitionToStopped();
    bool transitionToMovingDown();
    bool transitionToMovingUp();
    bool transitionToMovingRight();
    bool transitionToMovingLeft();
    bool transitionToAutoMovingUp();
    bool transitionToAutoMovingDown();
    bool transitionToFault();
    bool canMoveDown() { return sensors->getMinDistance() <= MAX_DISTANCE; };
    bool canMoveUp() { return wallDistanceCheck(); }
    bool isCloseToWall() { return sensors->getMinDistance() < MIN_SOFT_DIST_FROM_WALL;}
    bool canMoveLeft()  { return wallDistanceCheck() && sensors->getDistDiff() > MIN_SENSOR_DIFF; }
    bool canMoveRight()  { return wallDistanceCheck() && sensors->getDistDiff() < MAX_SENSOR_DIFF; }
    bool wallDistanceCheck() { return sensors->getMinDistance() > MIN_DIST_FROM_WALL; }
    bool isTvTurnedOn() { return digitalRead(TV_PIN) == TV_ON; }

    inline static const char * EventStrings[] =
        { "NONE", "DOWN_PRESSED", "UP_PRESSED", "RIGHT_PRESSED",
          "LEFT_PRESSED","FAULT_DETECTED", "BOTTOM_REACHED","TOP_REACHED",
          "RIGHT_REACHED", "LEFT_REACHED", "TV_TURNED_ON", "TV_TURNED_OFF" };
    inline static const char * StateStrings[] =
        { "STOPPED", "MOVING_DOWN", "MOVING_UP", "MOVING_RIGHT",
          "MOVING_LEFT","AUTO_MOVING_DOWN", "AUTO_MOVING_UP","FAULT" };
};

#endif