#include "debug.h"
#include "mount_state_machine.h"

MountStateMachine::MountStateMachine() {
  this->mountController = new MountController;
  this->remote = new Remote;
  this->sensors = new DistanceSensors;
  this->state = STOPPED;
  this->faultClearTimestamp = 0;
}

void MountStateMachine::begin() {
  Debug.println("Init state machine...");
  pinMode(TV_PIN, INPUT_PULLUP);
  pinMode(FAULT_LED, OUTPUT);
  mountController->begin();
  remote->begin();
  sensors->begin();
}

void MountStateMachine::refresh() {
  mountController->refresh();
  sensors->refresh();
}

void MountStateMachine::update() {
  refresh();
  Event event = getEvent();
  printInfo(event);
  transitionState(event);
}

MountStateMachine::Event MountStateMachine::getEvent() {
  sensors->refresh();
  Event events[8] = {
      getUpDownMotorFaultEvent(),
      getLeftRightMotorFaultEvent(),
      getUpperLimitEvent(),
      getLowerLimitEvent(),
      getLeftLimitEvent(),
      getRightLimitEvent(),
      getTvEvent(),
      getRemoteEvent()
  };
  // Find first non-NONE event or return the last one (remote-event)
  unsigned int matchingIdx = 7;
  for(unsigned int i = 0; i < 7; i++) {
    if (events[i] != NONE) {
      matchingIdx = i;
      break;
    }
  }
  return events[matchingIdx];
}

MountStateMachine::Event MountStateMachine::getUpDownMotorFaultEvent() {
  static unsigned long firstOverCurrentTimestamp = 0;

  if (state != MOVING_UP && state != AUTO_MOVING_UP && state != MOVING_DOWN && state != AUTO_MOVING_DOWN) {
    firstOverCurrentTimestamp = 0;
    return NONE;
  }

  int motorCurrent = mountController->getUpDownMotorCurrent();
  int maxCurrent = (state == MOVING_UP || state == AUTO_MOVING_UP) ? MAX_UP_CURRENT : MAX_DOWN_CURRENT;

  if (motorCurrent >= maxCurrent) {
    if (firstOverCurrentTimestamp == 0) {
      firstOverCurrentTimestamp = millis();
      return NONE;
    } else if(millis() - firstOverCurrentTimestamp > MAX_DURATION_WITH_OVER_CURRENT) {
      firstOverCurrentTimestamp = 0;
      return FAULT_DETECTED;
    }
  }

  return NONE;
}

MountStateMachine::Event MountStateMachine::getLeftRightMotorFaultEvent() {
  static unsigned long firstOverCurrentTimestamp = 0;

  if (state != MOVING_RIGHT && state != MOVING_LEFT) {
    firstOverCurrentTimestamp = 0;
    return NONE;
  }

  int motorCurrent = mountController->getLeftRightMotorCurrent();
  int maxCurrent = state == MOVING_RIGHT ? MAX_RIGHT_CURRENT : MAX_LEFT_CURRENT;

  if (motorCurrent >= maxCurrent) {
    if (firstOverCurrentTimestamp == 0) {
      firstOverCurrentTimestamp = millis();
      return NONE;
    } else if(millis() - firstOverCurrentTimestamp > MAX_DURATION_WITH_OVER_CURRENT) {
      firstOverCurrentTimestamp = 0;
      return FAULT_DETECTED;
    }
  }

  return NONE;
}

MountStateMachine::Event MountStateMachine::getTvEvent() {
  static bool prevTvOn = isTvTurnedOn();
  static unsigned long lastChangeTimestamp = 0;
  bool tvOn = isTvTurnedOn();
  // DEBOUNCE TV change event to prevent trigger on sudden pulses
  if (prevTvOn != tvOn) {
    if(lastChangeTimestamp == 0) {
      lastChangeTimestamp = millis();
    } else if (millis() - lastChangeTimestamp > TV_CHANGE_DEBOUNCE_DURATION) {
      Event event = tvOn ? TV_TURNED_ON : TV_TURNED_OFF;
      prevTvOn = tvOn;
      lastChangeTimestamp = 0;
      return event;
    }
  } else {
    lastChangeTimestamp = 0;
  }
  return NONE;
}

MountStateMachine::Event MountStateMachine::getUpperLimitEvent(){
  static bool prevMovementAllowed = canMoveUp();
  Event event = !canMoveUp() && prevMovementAllowed ? TOP_REACHED : NONE;
  prevMovementAllowed = canMoveUp();
  return event;
}

MountStateMachine::Event MountStateMachine::getRightLimitEvent() {
  static int prevMovementAllowed = canMoveRight();
  Event event = !canMoveRight() && prevMovementAllowed ? RIGHT_REACHED : NONE;
  prevMovementAllowed = canMoveRight();
  return event;
}

MountStateMachine::Event MountStateMachine::getLeftLimitEvent() {
  static int prevMovementAllowed = canMoveLeft();
  Event event = !canMoveLeft() && prevMovementAllowed ? LEFT_REACHED : NONE;
  prevMovementAllowed = canMoveLeft();
  return event;
}

MountStateMachine::Event MountStateMachine::getLowerLimitEvent(){
  // The motor has automatic cut off when it reached the bottom.
  // We use this to check if it has reached the bottom by checking zero current while it is moving down
  static unsigned long firstTimestampWithZeroCurrent = 0;

  if (state != MOVING_DOWN && state != AUTO_MOVING_DOWN) {
    firstTimestampWithZeroCurrent = 0;
    return NONE;
  }

  int motorCurrent = mountController->getUpDownMotorCurrent();
  if (motorCurrent <= ZERO_CURRENT_VALUE) {
    if (firstTimestampWithZeroCurrent == 0) {
      firstTimestampWithZeroCurrent = millis();
    } else if(millis() - firstTimestampWithZeroCurrent > MAX_DURATION_WITH_ZERO_CURRENT) {
      firstTimestampWithZeroCurrent = 0;
      return BOTTOM_REACHED;
    }
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

MountStateMachine::State MountStateMachine::getNextStateForFault(Event) const {
  return millis() < faultClearTimestamp ? FAULT : STOPPED;
}

bool MountStateMachine::transitionToStopped() {
  // If coming from FAULT, reset the LED
  if (state == FAULT) {
    digitalWrite(FAULT_LED, LOW);
  }
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
    isCloseToWall() ? mountController->moveDown(SLOW_UP_DOWN_DUTY_CYCLE) : mountController->moveDown();
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
    isCloseToWall() ? mountController->moveUp(SLOW_UP_DOWN_DUTY_CYCLE) : mountController->moveUp();
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
    isCloseToWall() ? mountController->moveUp(SLOW_UP_DOWN_DUTY_CYCLE) : mountController->moveUp();
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
    isCloseToWall() ? mountController->moveDown(SLOW_UP_DOWN_DUTY_CYCLE) : mountController->moveDown();
    state = AUTO_MOVING_DOWN;
  } else {
    transitionToStopped();
  }
  return state == AUTO_MOVING_DOWN;
}

bool MountStateMachine::transitionToFault() {
  // When fault first occurs, we set a faultClearTimeout
  if (state != FAULT) {
    digitalWrite(FAULT_LED, HIGH);
    faultClearTimestamp = millis() + FAULT_WAIT_DURATION;
  }
  mountController->stop();
  state = FAULT;
  return true;
}

void MountStateMachine::printInfo(Event event) {
  static char info[256];
  static const char fmt[] = "TV:%18s \r\nMotor1 Current:%6d \r\nMotor2 Current:%6d \r\nDist:%16d \r\nDist Diff:%11d \r\nState:%15s \r\nEvt:%17s";
  Debug.home();
  snprintf(info, sizeof(info), fmt,
           isTvTurnedOn() ? "ON" : "OFF",
           mountController->getUpDownMotorCurrent(),
           mountController->getLeftRightMotorCurrent(),
           sensors->getMinDistance(),
           sensors->getDistDiff(),
           getStateString(state),
           getEventString(event));
    Debug.println(info);
}
