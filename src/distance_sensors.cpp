#include "Arduino.h"
#include "debug.h"
#include "distance_sensors.h"

void DistanceSensors::begin() {
#ifdef USE_DISTANCE_SENSORS
  Sensor sensors[2] = {leftSensor, rightSensor};
  for(Sensor &sensor: sensors) {
    pinMode(sensor.controlPin, OUTPUT);
    digitalWrite(sensor.controlPin, LOW);
    delay(10);
  }

  // Change addresses of the sensors by waking one after another starting from zero
  for(Sensor &sensor: sensors) {
    Debug.print("Init sensor ");
    Debug.println(sensor.index);
    digitalWrite(sensor.controlPin, HIGH);
    delay(10);
    if (!sensor.driver.init()) {
      Debug.print("Failed init sensor ");
      Debug.println(sensor.index);
      while(1);
    }
    sensor.driver.setAddress(sensor.address);
    sensor.driver.setMeasurementTimingBudget(20000);
    delay(10);
  }

#endif
}

void DistanceSensors::refresh() {
#ifdef USE_DISTANCE_SENSORS
  Sensor sensors[2] = {leftSensor, rightSensor};
  static unsigned int readingIndex;
  for(Sensor &sensor: sensors) {
    unsigned int reading = sensor.driver.readRangeSingleMillimeters();
    if(!sensor.driver.timeoutOccurred()) {
      sensor.readings[readingIndex] = reading >= MAX_DISTANCE ? MAX_DISTANCE : reading;
    }
  }
  readingIndex++;
  if (readingIndex == DISTANCE_AVG_WINDOW_SIZE) {
    readingIndex = 0;
  }
#endif
}

unsigned int DistanceSensors::getMinDistance() {
  unsigned int minReading = MAX_DISTANCE;
#ifdef USE_DISTANCE_SENSORS
  uint_fast16_t leftDist = getLeftDist();
  uint_fast16_t rightDist = getRightDist();
  minReading = leftDist < rightDist ? leftDist : rightDist;
#endif
  return minReading;
}

int DistanceSensors::getDistDiff() {
  int leftMostDistance = 0, rightMostDistance = 0;
#ifdef USE_DISTANCE_SENSORS
  leftMostDistance = getLeftDist();
  rightMostDistance = getRightDist();
#endif
  return leftMostDistance - rightMostDistance;
}

unsigned int DistanceSensors::getRightDist() {
  uint_fast16_t sum = 0;
  for(auto reading: rightSensor.readings) {
    sum += reading;
  }
  return sum / DISTANCE_AVG_WINDOW_SIZE;
}

unsigned int DistanceSensors::getLeftDist() {
  uint_fast16_t sum = 0;
  for(auto reading: leftSensor.readings) {
    sum += reading;
  }
  return sum / DISTANCE_AVG_WINDOW_SIZE;
}

