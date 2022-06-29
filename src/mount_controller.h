#include "Arduino.h"
#include "motor_controller.h"

#ifndef MOUNT_CONTROLLER_H_

#define MOUNT_CONTROLLER_H_

#define USE_DISTANCE_SENSOR

#ifdef USE_DISTANCE_SENSOR
#include <Wire.h>
#include <Seeed_vl53l0x.h>
#endif

// define TV INPUTS
#define TV_PIN 21
#define TV_ON 0

// Distance sensor config
#define DISTANCE_AVG_WINDOW_SIZE 10

// MOTOR 1 CONFIG - UP DOWN ARM
#define MOTOR1_PWM_PIN 5
#define MOTOR1_IN_A_PIN 2
#define MOTOR1_SEL_A_PIN 3
#define MOTOR1_IN_B_PIN 4
#define MOTOR1_CURRENT_SENSE_INPUT A1
#define MOTOR1_DUTY_CYCLE 95
#define UP_DIR 1
#define DOWN_DIR 0

// MOTOR 2 CONFIG - LEFT RIGHT MOTION
#define MOTOR2_PWM_PIN 6
#define MOTOR2_IN_A_PIN 8
#define MOTOR2_SEL_A_PIN 7
#define MOTOR2_IN_B_PIN 9
#define MOTOR2_CURRENT_SENSE_INPUT A0
#define MOTOR2_DUTY_CYCLE 50
#define LEFT_DIR 0
#define RIGHT_DIR 1

class MountController {
  public:
    MountController();
    void setup();
    void moveUp() { upDownController->run(UP_DIR, MOTOR1_DUTY_CYCLE); };
    void moveDown() { upDownController->run(DOWN_DIR, MOTOR1_DUTY_CYCLE); };
    void moveLeft() { leftRightController->run(LEFT_DIR, MOTOR2_DUTY_CYCLE); };
    void moveRight() { leftRightController->run(RIGHT_DIR, MOTOR2_DUTY_CYCLE); };
    int getUpDownMotorCurrent() { return upDownController->getCurrent(); };
    int getLeftRightMotorCurrent() { return leftRightController->getCurrent(); };
    bool isTvTurnedOn() { return digitalRead(TV_PIN) == TV_ON; };
    void stop() { upDownController->stop(); leftRightController->stop(); };
    unsigned int getDistanceFromWall();
  private:
    MotorController *upDownController;
    MotorController *leftRightController;
#ifdef USE_DISTANCE_SENSOR
    Seeed_vl53l0x* sensor;
#endif
};

#endif