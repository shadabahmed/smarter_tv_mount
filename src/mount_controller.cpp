#include "debug.h"
#include "mount_controller.h"

void MountController::begin() {
  Debug.println("Init motor 1...");
  upDownController.begin();
  Debug.println("Init motor 2...");
  leftRightController.begin();
}

void MountController::refresh() {
  upDownController.refresh();
  leftRightController.refresh();
}

