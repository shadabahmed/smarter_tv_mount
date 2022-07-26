#ifndef DISTANCE_SENSORS_H
#define DISTANCE_SENSORS_H
#define USE_DISTANCE_SENSORS

#ifdef USE_DISTANCE_SENSORS
#define DISTANCE_SENSORS_COUNT 1
#define LEFT_SENSOR_ADDRESS 0x31
#define RIGHT_SENSOR_ADDRESS 0x32
#define LEFT_SENSOR_CONTROL_PIN 14
#define RIGHT_SENSOR_CONTROL_PIN 15
#include <Wire.h>
#include <VL53L0X.h>
#endif

#define MAX_DISTANCE 2000
#define DISTANCE_AVG_WINDOW_SIZE 10

struct Sensor {
  int controlPin;
  int address;
  VL53L0X driver;
};

class DistanceSensors {
public:
#ifdef USE_DISTANCE_SENSORS
  DistanceSensors() :
    leftSensor{LEFT_SENSOR_CONTROL_PIN, LEFT_SENSOR_ADDRESS },
    rightSensor{RIGHT_SENSOR_CONTROL_PIN, RIGHT_SENSOR_ADDRESS } {};
#else
  DistanceSensors()
#endif

  void begin();
  void refresh();
  unsigned int getMinDistance();
  int getDistDiff();
  private:
#ifdef USE_DISTANCE_SENSORS
  Sensor leftSensor;
  Sensor rightSensor;
  unsigned int leftReadings[DISTANCE_AVG_WINDOW_SIZE];
  unsigned int rightReadings[DISTANCE_AVG_WINDOW_SIZE];
#endif
};


#endif
