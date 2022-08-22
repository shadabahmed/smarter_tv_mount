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
  uint8_t controlPin;
  uint8_t address;
  uint8_t index;
  VL53L0X driver;
  uint16_t readings[DISTANCE_AVG_WINDOW_SIZE];
};

class DistanceSensors {
public:
#ifdef USE_DISTANCE_SENSORS
  DistanceSensors() :
    leftSensor{.controlPin = LEFT_SENSOR_CONTROL_PIN,
               .address = LEFT_SENSOR_ADDRESS,
               .index = 0,
               .driver = VL53L0X() },
    rightSensor{
    .controlPin = RIGHT_SENSOR_CONTROL_PIN,
    .address = RIGHT_SENSOR_ADDRESS,
    .index = 1,
    .driver = VL53L0X()} {};
#else
  DistanceSensors() = default;
#endif

  void begin();
  void refresh();
  unsigned int getLeftDist();
  unsigned int getRightDist();
  unsigned int getMinDistance();
  int getDistDiff();
  private:
#ifdef USE_DISTANCE_SENSORS
  Sensor leftSensor;
  Sensor rightSensor;
#endif
};


#endif
