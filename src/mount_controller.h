#include "Arduino.h"
#include "motor_controller.h"

#ifndef MOUNT_CONTROLLER_H_

#define MOUNT_CONTROLLER_H_

// MOTOR 1 CONFIG - UP DOWN ARM
#define MOTOR1_PWM_PIN 3
#define MOTOR1_IN_A_PIN 2
#define MOTOR1_SEL_0_PIN 5
#define MOTOR1_IN_B_PIN 4
#define MOTOR1_DUTY_CYCLE 100
#define MOTOR1_SLOW_DUTY_CYCLE 40
#define UP_DIR 1
#define DOWN_DIR 0

// MOTOR 2 CONFIG - LEFT RIGHT MOTION
#define MOTOR2_PWM_PIN 6
#define MOTOR2_IN_A_PIN 8
#define MOTOR2_SEL_0_PIN 7
#define MOTOR2_IN_B_PIN 9
#define MOTOR2_DUTY_CYCLE 90
#define LEFT_DIR 0
#define RIGHT_DIR 1

#ifdef ARDUINO_AVR_NANO_EVERY
#define MOTOR1_CURRENT_SENSE_INPUT A7
#define MOTOR2_CURRENT_SENSE_INPUT A6
#elif ARDUINO_NANO_RP2040_CONNECT
#define MOTOR1_CURRENT_SENSE_INPUT A7.get()
#define MOTOR2_CURRENT_SENSE_INPUT A6.get()
#endif

#define FAULT_LED LED_BUILTIN

class MountController {
  public:
    MountController();
    void begin();
    void refresh();
    void moveUp(int dutyCycle = MOTOR1_DUTY_CYCLE) { upDownController->run(UP_DIR, dutyCycle); };
    void moveDown(int dutyCycle = MOTOR1_DUTY_CYCLE) { upDownController->run(DOWN_DIR, dutyCycle); };
    void moveLeft(int dutyCycle = MOTOR2_DUTY_CYCLE) { leftRightController->run(LEFT_DIR, dutyCycle); };
    void moveRight(int dutyCycle = MOTOR2_DUTY_CYCLE) { leftRightController->run(RIGHT_DIR, dutyCycle); };
    int getUpDownMotorCurrent() { return upDownController->getCurrent(); };
    int getLeftRightMotorCurrent() { return leftRightController->getCurrent(); };
    void stop() { upDownController->stop(); leftRightController->stop(); };
  private:
    MotorController *upDownController;
    MotorController *leftRightController;
};

#endif