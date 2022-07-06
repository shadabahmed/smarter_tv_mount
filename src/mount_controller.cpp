#include "debug.h"
#include "mount_controller.h"

MountController::MountController() {
  this->upDownController = new MotorController(MOTOR1_SEL_A_PIN, MOTOR1_IN_A_PIN, MOTOR1_IN_B_PIN, MOTOR1_PWM_PIN, MOTOR1_CURRENT_SENSE_INPUT);
  this->leftRightController = new MotorController(MOTOR2_SEL_A_PIN, MOTOR2_IN_A_PIN, MOTOR2_IN_B_PIN, MOTOR2_PWM_PIN, MOTOR2_CURRENT_SENSE_INPUT);
}

void MountController::begin() {
  Debug.println("Init motor 1...");
  upDownController->begin();
  Debug.println("Init motor 2...");
  leftRightController->begin();
}

void MountController::refresh() {
  upDownController->refresh();
  leftRightController->refresh();
}

