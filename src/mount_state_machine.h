#ifndef MOUNT_STATE_MACHINE_H_
#define MOUNT_STATE_MACHINE_H_

#include "mount_controller.h"
#include "remote.h"
#define MAX_UP_CURRENT 100
#define MAX_DOWN_CURRENT 100
#define MAX_LEFT_CURRENT 100
#define MAX_RIGHT_CURRENT 100
#define MAX_TICKS_WITH_OVER_CURRENT 5
#define ZERO_CURRENT 2
#define MAX_TICKS_WITH_ZERO_CURRENT 5
#define MIN_DIST_FROM_WALL 100
#define SETUP_WAIT 1000
#define TICK 20
#define TICKS_TO_WAIT_AFTER_FAULT 2000 / TICK

class MountStateMachine {
  public: enum Event { NONE, DOWN_PRESSED, UP_PRESSED, RIGHT_PRESSED,
        LEFT_PRESSED, FAULT_DETECTED, DOWN_REACHED, UP_REACHED,
        RIGHT_REACHED, LEFT_REACHED, TV_TURNED_ON, TV_TURNED_OFF};
  public: enum State { READY, MOVING_DOWN, MOVING_UP, MOVING_RIGHT,
          MOVING_LEFT, AUTO_MOVING_DOWN, AUTO_MOVING_UP, FAULT };

  public:
    MountStateMachine();
    State getState() { return this->state; };
    Event getEvent();
    void setup();
    bool updateState(Event);
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
    State updateReadyState(Event);
    State updateMovingDownState(Event);
    State updateMovingUpState(Event);
    State updateMovingRightState(Event);
    State updateMovingLeftState(Event);
    State updateAutoMovingUpState(Event);
    State updateAutoMovingDownState(Event);
    State updateFaultState(Event);
    inline static char * EventStrings[] = { "NONE", "DOWN_PRESSED", "UP_PRESSED", "RIGHT_PRESSED",
                                   "LEFT_PRESSED","FAULT_DETECTED", "DOWN_REACHED","UP_REACHED",
                                   "RIGHT_REACHED", "LEFT_REACHED", "TV_TURNED_ON", "TV_TURNED_OFF" };
    inline static char * StateStrings[] = { "READY", "MOVING_DOWN", "MOVING_UP", "MOVING_RIGHT",
                                   "MOVING_LEFT","AUTO_MOVING_DOWN", "AUTO_MOVING_UP","FAULT" };
};

#endif