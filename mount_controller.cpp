
#include "mount_controller.h"


MountController::MountController() {
  upDownController = new MotorController(MOTOR1_SEL_A_PIN, MOTOR1_IN_A_PIN, MOTOR1_IN_B_PIN, MOTOR1_PWM_PIN, MOTOR1_CURRENT_SENSE_INPUT);
  leftRightController = new MotorController(MOTOR2_SEL_A_PIN, MOTOR2_IN_A_PIN, MOTOR2_IN_B_PIN, MOTOR2_PWM_PIN, MOTOR2_CURRENT_SENSE_INPUT);
  remote = new Remote();
  lox = new Adafruit_VL53L0X();
  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(TV_PIN, INPUT);
  state = READY;
}

void MountController::setup(){ 
  upDownController->setup();
  leftRightController->setup();
}

MountController::Event MountController::getEvent() {
  Event upDownMotorEvent = getMotorEvent(this->upDownController);
  if(upDownMotorEvent != NONE) {
    return upDownMotorEvent;
  }
  
  Event leftRightMotorEvent = getMotorEvent(this->leftRightController); 
  if(leftRightMotorEvent != NONE) {
    return leftRightMotorEvent;
  }
  
  Event tvEvent = getTvEvent();
  if(tvEvent != NONE){
    return tvEvent;    
  }
  
  Event upperLimitEvent = getUpperLimitEvent();
  if(upperLimitEvent != NONE) {
    return upperLimitEvent;
  }
  
  Event lowerLimitEvent = getLowerLimitEvent();
  if(lowerLimitEvent != NONE) {
    return lowerLimitEvent;
  }

  return getRemoteEvent();
}

MountController::Event MountController::getMotorEvent(MotorController * motorController) {
  motorController->monitor();
  if (motorController->isFaultDetected()) {
    return FAULT_DETECTED;
  }
  return NONE;
}

MountController::Event MountController::getTvEvent() {
  static int tvPin = TV_OFF;
  int prev = tvPin;
  tvPin = digitalRead(TV_PIN);
  Serial.print(digitalRead(TV_PIN));
  if (prev == TV_OFF && tvPin == TV_ON){
    return TV_TURNED_ON;
  } else if(prev == TV_ON && tvPin == TV_OFF) {
    return TV_TURNED_OFF;
  }
  return NONE;
}

MountController::Event MountController::getUpperLimitEvent(){
  return NONE;
  if (this->getDistanceFromWall() <= MIN_DIST_FROM_WALL){
    return UP_REACHED;
  }
  return NONE;
}

MountController::Event MountController::getLowerLimitEvent(){
  return NONE;
}

MountController::Event MountController::getRemoteEvent(){
  if (remote->isButtonPressed()) {
    switch (remote->getButtonCode()) {
      case 8: return UP_BUTTON_PRESSED;
      case 4: return DOWN_BUTTON_PRESSED;
      case 2: return LEFT_BUTTON_PRESSED;
      case 1: return RIGHT_BUTTON_PRESSED;
    }
  }
  return NONE;
}

void MountController::moveUp() {
  upDownController->run(UP_DIR);
}

void MountController::moveDown() {
  upDownController->run(DOWN_DIR);
}

void MountController::moveLeft() {
  upDownController->run(LEFT_DIR);
}

void MountController::moveRight() {
  upDownController->run(RIGHT_DIR);
}

int MountController::getUpDownMotorCurrent() {
  return upDownController->getCurrent();
}

int MountController::getLeftRightMotorCurrent() {
  return upDownController->getCurrent();
}

void MountController::resetFault() {
  upDownController->resetFault();
}

bool MountController::isFaultDetected() {
  return upDownController->isFaultDetected();
}


void MountController::stop() {
  upDownController->stop();
}

int MountController::getDistanceFromWall() {
  static unsigned int counter = 0;
  static unsigned int sum = 0;
  VL53L0X_RangingMeasurementData_t measure;
  lox->rangingTest(&measure, false);
  sum += measure.RangeMilliMeter;
  if (counter < DISTANCE_AVG_WINDOW_SIZE) {
    return 10000;
  }  
  int avgDist = sum / counter;
  counter = 0;
  sum = 0;
  return avgDist;
}


