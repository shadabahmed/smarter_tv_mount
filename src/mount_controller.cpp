#include "debug.h"
#include "mount_controller.h"

MountController::MountController() {
  this->upDownController = new MotorController(MOTOR1_SEL_A_PIN, MOTOR1_IN_A_PIN, MOTOR1_IN_B_PIN, MOTOR1_PWM_PIN, MOTOR1_CURRENT_SENSE_INPUT);
  this->leftRightController = new MotorController(MOTOR2_SEL_A_PIN, MOTOR2_IN_A_PIN, MOTOR2_IN_B_PIN, MOTOR2_PWM_PIN, MOTOR2_CURRENT_SENSE_INPUT);
#if defined(USE_DISTANCE_SENSOR)
  this->sensor = new Seeed_vl53l0x();
#endif
}

void MountController::setup() {
  Debug::println("Init motor 1...");
  upDownController->setup();
  Debug::println("Init motor 2...");
  leftRightController->setup();
#if defined(USE_DISTANCE_SENSOR)
  Debug::println("Init Distance Sensor...");
  VL53L0X_Error Status = VL53L0X_ERROR_NONE;
  Status = sensor->VL53L0X_common_init();
  if (VL53L0X_ERROR_NONE != Status) {
    Debug::println("Failed to boot VL53L0X");
    while (1);
  }
  sensor->VL53L0X_high_speed_ranging_init();
#endif
}

unsigned int MountController::getDistanceFromWall() {
#if defined(USE_DISTANCE_SENSOR)
  static int index = 0;
  static unsigned int readings[DISTANCE_AVG_WINDOW_SIZE];
  VL53L0X_RangingMeasurementData_t RangingMeasurementData;
  VL53L0X_Error Status = VL53L0X_ERROR_NONE;

  memset(&RangingMeasurementData, 0, sizeof(VL53L0X_RangingMeasurementData_t));
  Status = sensor->PerformSingleRangingMeasurement(&RangingMeasurementData);

  if(VL53L0X_ERROR_NONE == Status) {
    readings[index++] = RangingMeasurementData.RangeMilliMeter >= 2000 ? 2000 : RangingMeasurementData.RangeMilliMeter;
    index = index == DISTANCE_AVG_WINDOW_SIZE ? 0 : index;
  }
  unsigned int sum = 0;
  for(int i = 0; i < DISTANCE_AVG_WINDOW_SIZE; i++) {
    // If there is no reading at this slot, put in maximum value to read distance as really far. Not to trigger up reached immediately
    if (readings[i] == 0) {
      readings[i] = 2000;
    }
    sum += readings[i];
  }

  return sum / DISTANCE_AVG_WINDOW_SIZE;
#else
  return 2000;
#endif
}


