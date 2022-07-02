#ifndef DISTANCE_SENSORS_H
#define DISTANCE_SENSORS_H
#define USE_DISTANCE_SENSORS

#ifdef USE_DISTANCE_SENSORS
#define DISTANCE_SENSORS_COUNT 2
#define DISTANCE_SENSORS_ADDRESS_START_ADDRESS 0x30
const int DISTANCE_SENSORS_CONTROL_PINS[] = {25, 22};
#include <Wire.h>
#include <Adafruit_VL53L0X.h>
#endif

#define MAX_DISTANCE 2000
#define DISTANCE_AVG_WINDOW_SIZE 10

class DistanceSensors {
  public:
  DistanceSensors();
  void begin();
  void refresh();
  int getMinDistance();
  int getDistDiff();
  private:
  int readings[DISTANCE_SENSORS_COUNT][DISTANCE_AVG_WINDOW_SIZE];
#ifdef USE_DISTANCE_SENSORS
  Adafruit_VL53L0X* sensors[DISTANCE_SENSORS_COUNT];
#endif
};


#endif //TV_MOUNT_DISTANCE_SENSORS_H
