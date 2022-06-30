#include "debug.h"
#include "mount_state_machine.h"

MountStateMachine::MountStateMachine() {
  this->mountController = new MountController();
  this->remote = new Remote();
  this->state = STOPPED;
}

void MountStateMachine::setup() {
  Debug::println("Init state machine...");
  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(TV_PIN, INPUT_PULLUP);
  mountController->setup();
  Debug::println("Init remote...");
  remote->setup();
}

MountStateMachine::Event MountStateMachine::getEvent() {
  Event upDownMotorEvent = getUpDownMotorEvent();
  if(upDownMotorEvent != NONE) {
    return upDownMotorEvent;
  }
  
  Event leftRightMotorEvent = getLeftRightMotorEvent(); 
  if(leftRightMotorEvent != NONE) {
    return leftRightMotorEvent;
  }

  Event upperLimitEvent = getUpperLimitEvent();
  if(upperLimitEvent != NONE) {
    return upperLimitEvent;
  }
  
  Event lowerLimitEvent = getLowerLimitEvent();
  if(lowerLimitEvent != NONE) {
    return lowerLimitEvent;
  }
  
  Event tvEvent = getTvEvent();
  if(tvEvent != NONE){
    return tvEvent;    
  }
  return getRemoteEvent();
}

MountStateMachine::Event MountStateMachine::getUpDownMotorEvent() {
  static int ticksWithOverCurrent = 0;

  if (state == STOPPED) {
    ticksWithOverCurrent = 0;
    return NONE;
  }

  int motorCurrent = mountController->getUpDownMotorCurrent();
  if (state == MOVING_UP || state == AUTO_MOVING_UP) {
    if ( motorCurrent >= MAX_UP_CURRENT) {
      ticksWithOverCurrent++;
    } else if(ticksWithOverCurrent > 0) {
      ticksWithOverCurrent--;
    }
  }

  if (state == MOVING_DOWN || state == AUTO_MOVING_DOWN) {
    if (motorCurrent >= MAX_DOWN_CURRENT) {
      ticksWithOverCurrent++;
    } else if(ticksWithOverCurrent > 0) {
      ticksWithOverCurrent--;
    }
  }

  if (ticksWithOverCurrent == MAX_TICKS_WITH_OVER_CURRENT) {
    ticksWithOverCurrent = 0;
    return FAULT_DETECTED;
  }

  return NONE;
}

MountStateMachine::Event MountStateMachine::getLeftRightMotorEvent() {
  static int ticksWithOverCurrent = 0;

  if (state == STOPPED) {
    ticksWithOverCurrent = 0;
    return NONE;
  }

  int motorCurrent = mountController->getLeftRightMotorCurrent();
  if (state == MOVING_LEFT) {
    if ( motorCurrent >= MAX_LEFT_CURRENT) {
      ticksWithOverCurrent++;
    } else if(ticksWithOverCurrent > 0) {
      ticksWithOverCurrent--;
    }
  }

  if (state == MOVING_RIGHT) {
    if (motorCurrent >= MAX_RIGHT_CURRENT) {
      ticksWithOverCurrent++;
    } else if(ticksWithOverCurrent > 0) {
      ticksWithOverCurrent--;
    }
  }

  if (ticksWithOverCurrent == MAX_TICKS_WITH_OVER_CURRENT) {
    ticksWithOverCurrent = 0;
    return FAULT_DETECTED;
  }

  return NONE;
}

MountStateMachine::Event MountStateMachine::getTvEvent() {
  static bool tvOn = mountController->isTvTurnedOn();
  static bool prevTvOn = tvOn;
  static int ticks = 0;
  tvOn = mountController->isTvTurnedOn();
  // DEBOUNCE TV change event to prevent trigger on sudden pulses
  if (prevTvOn != tvOn) {
    ticks++;
    if (ticks == TICKS_TO_WAIT_FOR_TV_ON) {
      Event event = tvOn ? TV_TURNED_ON : TV_TURNED_OFF;
      prevTvOn = tvOn;
      ticks = 0;
      return event;
    }
  } else {
    ticks = 0;
  }
  return NONE;
}

MountStateMachine::Event MountStateMachine::getUpperLimitEvent(){
  static Event prevEvent = NONE;
  unsigned int distanceFromWall = mountController->getDistanceFromWall(true);
  if (prevEvent != UP_REACHED && distanceFromWall <= MIN_DIST_FROM_WALL){
    return prevEvent = UP_REACHED;
  } else if (distanceFromWall > MIN_DIST_FROM_WALL) {
    return prevEvent = NONE;
  }
  return NONE;
}

MountStateMachine::Event MountStateMachine::getLowerLimitEvent(){
  static int ticksWithZeroCurrent = 0;

  if (state == STOPPED) {
    ticksWithZeroCurrent = 0;
    return NONE;
  }

  int motorCurrent = mountController->getUpDownMotorCurrent();
  if (state == AUTO_MOVING_DOWN || state == MOVING_DOWN) {
    if (motorCurrent <= ZERO_CURRENT_VALUE) {
      ticksWithZeroCurrent++;
    } else if(ticksWithZeroCurrent > 0) {
      ticksWithZeroCurrent--;
    }
  }

  if (ticksWithZeroCurrent == MAX_TICKS_WITH_ZERO_CURRENT) {
    ticksWithZeroCurrent = 0;
    return DOWN_REACHED;
  }

  return NONE;
}

MountStateMachine::Event MountStateMachine::getRemoteEvent(){
  if (remote->isButtonPressed()) {
    switch (remote->getButtonCode()) {
      case 8: return UP_PRESSED;
      case 4: return DOWN_PRESSED;
      case 2: return LEFT_PRESSED;
      case 1: return RIGHT_PRESSED;
    }
  }
  return NONE;
}

bool MountStateMachine::transitionState(Event event) {
   State nextState = getNextState(event);
   switch (nextState) {
    case STOPPED: return transitionToStopped();
    case MOVING_DOWN: return transitionToMovingDown();
    case MOVING_UP: return transitionToMovingUp();
    case MOVING_RIGHT: return transitionToMovingRight();
    case MOVING_LEFT: return transitionToMovingLeft();
    case AUTO_MOVING_UP: return transitionToAutoMovingUp();
    case AUTO_MOVING_DOWN: return transitionToAutoMovingDown();
    case FAULT: return transitionToFault();
    default: return false;
  }
}

MountStateMachine::State MountStateMachine::getNextState(Event event) {
  switch (this->state) {
    case STOPPED: return getNextStateForStopped(event);
    case MOVING_DOWN: return getNextStateForMovingDown(event);
    case MOVING_UP: return getNextStateForMovingUp(event);
    case MOVING_RIGHT: return getNextStateForMovingRight(event);
    case MOVING_LEFT: return getNextStateForMovingLeft(event);
    case AUTO_MOVING_UP: return getNextStateForAutoMovingUp(event);
    case AUTO_MOVING_DOWN: return getNextStateForAutoMovingDown(event);
    case FAULT: return getNextStateForFault(event);
    default: return state;
  }
}

MountStateMachine::State MountStateMachine::getNextStateForStopped(Event event) {
  switch(event) {
    case TV_TURNED_OFF: return AUTO_MOVING_UP;
    case UP_PRESSED: return MOVING_UP;
    case TV_TURNED_ON: return AUTO_MOVING_DOWN;
    case DOWN_PRESSED: return MOVING_DOWN;
    case LEFT_PRESSED: return MOVING_LEFT;
    case RIGHT_PRESSED: return MOVING_RIGHT;
    default: return STOPPED;
  }
}

MountStateMachine::State MountStateMachine::getNextStateForMovingDown(Event event) {
  switch(event) {
    case FAULT_DETECTED: return FAULT;
    case TV_TURNED_OFF: return AUTO_MOVING_UP;
    case DOWN_PRESSED: return MOVING_DOWN;
    default: return STOPPED;
  }
}

MountStateMachine::State MountStateMachine::getNextStateForMovingUp(Event event) {
  switch(event) {
    case FAULT_DETECTED: return FAULT;
    case UP_PRESSED: return MOVING_UP;
    default: return STOPPED;
  }
}

MountStateMachine::State MountStateMachine::getNextStateForMovingRight(Event event) {
  switch(event) {
    case FAULT_DETECTED: return FAULT;
    case RIGHT_PRESSED: return MOVING_RIGHT;
    default: return STOPPED;
  }
}

MountStateMachine::State MountStateMachine::getNextStateForMovingLeft(Event event) {
  switch(event) {
    case FAULT_DETECTED: return FAULT;
    case LEFT_PRESSED: return MOVING_LEFT;
    default: return STOPPED;
  }
}

MountStateMachine::State MountStateMachine::getNextStateForAutoMovingDown(Event event) {
  switch(event) {
    case NONE:
    case TV_TURNED_ON: return AUTO_MOVING_DOWN;
    case FAULT_DETECTED: return FAULT;
    case TV_TURNED_OFF: return AUTO_MOVING_UP;
    default: return STOPPED;
  }
}

MountStateMachine::State MountStateMachine::getNextStateForAutoMovingUp(Event event) {
  switch(event) {
    case NONE:
    case TV_TURNED_OFF: return AUTO_MOVING_UP;
    case FAULT_DETECTED: return FAULT;
    case TV_TURNED_ON: return AUTO_MOVING_DOWN;
    default: return STOPPED;
  }
}

MountStateMachine::State MountStateMachine::getNextStateForFault(Event) {
  static int ticks = 0;
  digitalWrite(LED_BUILTIN, 1);
  if (ticks++ == TICKS_TO_WAIT_AFTER_FAULT) {
    ticks = 0;
    digitalWrite(LED_BUILTIN, 0);
    return STOPPED;
  }
  return FAULT;
}

bool MountStateMachine::transitionToStopped() {
  mountController->stop();
  state = STOPPED;
  return true;
}

bool MountStateMachine::transitionToMovingDown() {
  if (canMoveDown()) {
    // If we are just starting in this state, stop motor to reset motor current readings
    if (state != MOVING_DOWN) {
      mountController->stop();
    }
    mountController->moveDown();
    state = MOVING_DOWN;
  } else {
    transitionToStopped();
  }
  return state == MOVING_DOWN;
}

bool MountStateMachine::transitionToMovingUp() {
  if (canMoveUp()) {
    // If we are just starting in this state, stop motor to reset motor current readings
    if (state != MOVING_UP) {
      mountController->stop();
    }
    shouldSlowMoveUp() ? mountController->moveUpSlow() : mountController->moveUp();
    state = MOVING_UP;
  } else {
    transitionToStopped();
  }
  return state == MOVING_UP;
}

bool MountStateMachine::transitionToMovingRight() {
  if (canMoveRight()) {
    if (state != MOVING_RIGHT) {
      mountController->stop();
    }
    mountController->moveRight();
    state = MOVING_RIGHT;
  } else {
    transitionToStopped();
  }
  return state == MOVING_RIGHT;
}

bool MountStateMachine::transitionToMovingLeft() {
  if (canMoveLeft()) {
    if (state != MOVING_LEFT) {
      mountController->stop();
    }
    mountController->moveLeft();
    state = MOVING_LEFT;
  } else {
    transitionToStopped();
  }
  return state == MOVING_LEFT;
}

bool MountStateMachine::transitionToAutoMovingUp() {
  if (canMoveUp()) {
    if (state != AUTO_MOVING_UP) {
      mountController->stop();
    }
    shouldSlowMoveUp() ? mountController->moveUpSlow() : mountController->moveUp();
    state = AUTO_MOVING_UP;
  } else {
    transitionToStopped();
  }
  return state == AUTO_MOVING_UP;
}

bool MountStateMachine::transitionToAutoMovingDown() {
  if (canMoveDown()) {
    if (state != AUTO_MOVING_DOWN) {
      mountController->stop();
    }
    mountController->moveDown();
    state = AUTO_MOVING_DOWN;
  } else {
    transitionToStopped();
  }
  return state == AUTO_MOVING_DOWN;
}

bool MountStateMachine::transitionToFault() {
  mountController->stop();
  state = FAULT;
  return true;
}

void MountStateMachine::printInfo(Event event) {
  static char info[144];
  static const char fmt[] = "TV:%18s\r\nMotor1 Current:%6d\r\nMotor2 Current:%6d\r\nDist:%16d\r\nState:%15s\r\nEvt:%17s";
  Debug::home();
  snprintf(info, sizeof(info), fmt,
           mountController->isTvTurnedOn() ? "ON" : "OFF",
           mountController->getUpDownMotorCurrent(),
           mountController->getLeftRightMotorCurrent(),
           mountController->getDistanceFromWall(),
           getStateString(getState()),
           getEventString(event));
    Debug::println(info);
}

