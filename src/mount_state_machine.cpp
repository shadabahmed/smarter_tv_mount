#include "debug.h"
#include "mount_state_machine.h"

MountStateMachine::MountStateMachine() {
  this->mountController = new MountController();
  this->remote = new Remote();
  this->state = READY;
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

bool MountStateMachine::updateState(Event event) {
   State nextState = getNextState(event);
   if (nextState == state) {
     return false;
   }
   state = nextState;
   return true;
}

MountStateMachine::State MountStateMachine::getNextState(Event event) {
  switch (this->state) {
    case READY: return updateReadyState(event);
    case MOVING_DOWN: return updateMovingDownState(event);
    case MOVING_UP: return updateMovingUpState(event);
    case MOVING_RIGHT: return updateMovingRightState(event);
    case MOVING_LEFT: return updateMovingLeftState(event);
    case AUTO_MOVING_UP: return updateAutoMovingUpState(event);
    case AUTO_MOVING_DOWN: return updateAutoMovingDownState(event);
    case FAULT: return updateFaultState(event);
    default: return state;
  }
}

MountStateMachine::State MountStateMachine::updateReadyState(Event event) {
  switch(event) {
    case TV_TURNED_OFF: mountController->moveUp(); return AUTO_MOVING_UP;
    case UP_PRESSED: mountController->moveUp(); return MOVING_UP;
    case TV_TURNED_ON: mountController->moveDown(); return AUTO_MOVING_DOWN;
    case DOWN_PRESSED: mountController->moveDown(); return MOVING_DOWN;
    case LEFT_PRESSED: mountController->moveLeft(); return MOVING_LEFT;
    case RIGHT_PRESSED: mountController->moveRight(); return MOVING_RIGHT;
  }
  return READY;
}

MountStateMachine::State MountStateMachine::updateMovingDownState(Event event) {
  switch(event) {
    case NONE:
    case DOWN_REACHED: mountController->stop(); return READY;
    case FAULT_DETECTED: mountController->stop(); return FAULT;
    case TV_TURNED_OFF: mountController->moveUp(); return AUTO_MOVING_UP;
    default: return MOVING_DOWN;
  }
}

MountStateMachine::State MountStateMachine::updateMovingUpState(Event event) {
  switch(event) {
    case NONE:
    case UP_REACHED: mountController->stop(); return READY;
    case FAULT_DETECTED: mountController->stop(); return FAULT;
    case TV_TURNED_ON: mountController->moveDown(); return AUTO_MOVING_DOWN;
    default: return MOVING_UP;
  }
}

MountStateMachine::State MountStateMachine::updateMovingRightState(Event event) {
  switch(event) {
    case NONE:
    case RIGHT_REACHED: mountController->stop(); return READY;
    case FAULT_DETECTED: mountController->stop(); return FAULT;
    case TV_TURNED_ON: mountController->moveDown(); return AUTO_MOVING_DOWN;
    case TV_TURNED_OFF: mountController->moveUp(); return AUTO_MOVING_UP;
    default: return MOVING_RIGHT;
  }
}

MountStateMachine::State MountStateMachine::updateMovingLeftState(Event event) {
  switch(event) {
    case NONE:
    case LEFT_REACHED: mountController->stop(); return READY;
    case FAULT_DETECTED: mountController->stop(); return FAULT;
    case TV_TURNED_ON: mountController->moveDown(); return AUTO_MOVING_DOWN;
    case TV_TURNED_OFF: mountController->moveUp(); return AUTO_MOVING_UP;
    default: return MOVING_LEFT;
  }
}

MountStateMachine::State MountStateMachine::updateAutoMovingDownState(Event event) {
  switch(event) {
    case DOWN_REACHED: mountController->stop(); return READY;
    case FAULT_DETECTED: mountController->stop(); return FAULT;
    case TV_TURNED_OFF: mountController->moveUp(); return AUTO_MOVING_UP;
    case UP_PRESSED:
    case DOWN_PRESSED:
    case LEFT_PRESSED:
    case RIGHT_PRESSED: mountController->stop(); return READY;
    case NONE:
    default: return AUTO_MOVING_DOWN;
  }
}

MountStateMachine::State MountStateMachine::updateAutoMovingUpState(Event event) {
  switch(event) {
    case UP_REACHED: mountController->stop(); return READY;
    case FAULT_DETECTED: mountController->stop(); return FAULT;
    case TV_TURNED_ON: mountController->moveDown(); return AUTO_MOVING_DOWN;
    case UP_PRESSED:
    case DOWN_PRESSED:
    case LEFT_PRESSED:
    case RIGHT_PRESSED: mountController->stop(); return READY;
    case NONE:
    default: return AUTO_MOVING_UP;
  }
}

MountStateMachine::State MountStateMachine::updateFaultState(Event _) {
  static int ticks = 0;
  digitalWrite(LED_BUILTIN, 1);
  if (ticks++ == TICKS_TO_WAIT_AFTER_FAULT) {
    ticks = 0;
    digitalWrite(LED_BUILTIN, 0);
    return READY;
  }
  return FAULT;
}

MountStateMachine::Event MountStateMachine::getUpDownMotorEvent() {
  static int ticksWithOverCurrent = 0;

  if (state == READY) {
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

  if (state == READY) {
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
  bool prevTvOn = tvOn;
  tvOn = mountController->isTvTurnedOn();
  if (!prevTvOn && tvOn){
    return TV_TURNED_ON;
  } else if(prevTvOn && !tvOn) {
    return TV_TURNED_OFF;
  }
  return NONE;
}

MountStateMachine::Event MountStateMachine::getUpperLimitEvent(){
  static Event prevEvent = NONE;
  if (prevEvent != UP_REACHED && mountController->getDistanceFromWall() <= MIN_DIST_FROM_WALL){
    return prevEvent = UP_REACHED;
  } else if (mountController->getDistanceFromWall() > MIN_DIST_FROM_WALL) {
    return prevEvent = NONE;
  }
  return NONE;
}

MountStateMachine::Event MountStateMachine::getLowerLimitEvent(){
  static int ticksWithZeroCurrent = 0;

  if (state == READY) {
    ticksWithZeroCurrent = 0;
    return NONE;
  }

  int motorCurrent = mountController->getUpDownMotorCurrent();
  if (state == AUTO_MOVING_DOWN || state == MOVING_DOWN) {
    if (motorCurrent <= ZERO_CURRENT) {
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

void MountStateMachine::printInfo(Event event) {
  char info[144];
  __attribute__((address(0x100))) const char fmt[] = "TV:%18s\r\nMotor1 Current:%6d\r\nMotor2 Current:%6d\r\nDist:%16d\r\nState:%16s\r\nEvt:%17s";
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


