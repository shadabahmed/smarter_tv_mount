
#include "Arduino.h"
#include "motor_controller.h"
#include "Adafruit_VL53L0X.h"
#include "remote.h"

#ifndef MOUNT_COTROLLER_H

#define MOUNT_COTROLLER_H

// define TV INPUTS
#define TV_PIN 21
#define TV_OFF 1
#define TV_ON 0

// Distance sensor config
#define DISTANCE_AVG_WINDOW_SIZE 10
#define MIN_DIST_FROM_WALL 50

// MOTOR 1 INPUTS
#define MOTOR1_PWM_PIN 5
#define MOTOR1_IN_A_PIN 2
#define MOTOR1_SEL_A_PIN 3
#define MOTOR1_IN_B_PIN 4


// MOTOR 1 CURRENT SENSE
#define MOTOR1_CURRENT_SENSE_INPUT A1

#define UP_DIR 1
#define DOWN_DIR 0

// MOTOR 2 INPUTS
#define MOTOR2_PWM_PIN 6
#define MOTOR2_IN_A_PIN 8
#define MOTOR2_SEL_A_PIN 7
#define MOTOR2_IN_B_PIN 9


// MOTOR 1 CURRENT SENSE
#define MOTOR2_CURRENT_SENSE_INPUT A0

#define LEFT_DIR 1
#define RIGHT_DIR 0

static const char * EventStrings[] = { "NONE", "UP_BUTTON_PRESSED", "DOWN_BUTTON_PRESSED", "LEFT_BUTTON_PRESSED", "RIGHT_BUTTON_PRESSED", "FAULT_DETECTED", "UP_REACHED", "DOWN_REACHED", "TV_TURNED_ON", "TV_TURNED_OFF" };
static const char * StateStrings[] = { "READY", "MOVING_DOWN", "MOVING_UP", "AUTO_MOVING_DOWN", "AUTO_MOVING_UP", "MOVING_RIGHT", "MOVING_LEFT", "FAULT" };

class MountController {
  public: enum Event { NONE, UP_BUTTON_PRESSED, DOWN_BUTTON_PRESSED, LEFT_BUTTON_PRESSED, RIGHT_BUTTON_PRESSED, FAULT_DETECTED, UP_REACHED, DOWN_REACHED, TV_TURNED_ON, TV_TURNED_OFF};
  public: enum State { READY, MOVING_DOWN, MOVING_UP, AUTO_MOVING_DOWN, AUTO_MOVING_UP, MOVING_RIGHT, MOVING_LEFT, FAULT };

  public:
    MountController();
    void setup();
    State getState() { return this->state; };
    Event getEvent();    
    const char * getStateString(State event) { return StateStrings[state]; };
    const char * getEventString(Event event) { return EventStrings[event]; };
    void moveUp();
    void moveDown();
    void moveLeft();
    void moveRight();
    void stop();
    void monitor();
    int getUpDownMotorCurrent();
    int getLeftRightMotorCurrent();
    bool isFaultDetected();
    void resetFault();
    
  private:
    MotorController *upDownController;
    MotorController *leftRightController;
    Adafruit_VL53L0X *lox;
    Remote *remote;
    State state;    
    int getDistanceFromWall();
    Event getTvEvent();
    Event getRemoteEvent();    
    Event getMotorEvent(MotorController *);    
    Event getUpperLimitEvent();  
    Event getLowerLimitEvent();  
};

#endif