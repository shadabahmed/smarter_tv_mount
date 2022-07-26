#include "Arduino.h"
#include "debug.h"
#include "distance_sensors.h"
#include <list>

void DistanceSensors::begin() {
#ifdef USE_DISTANCE_SENSORS
  for(auto sensor: std::list<Sensor>(leftSensor, rightSensor)) {
    pinMode(sensor.controlPin, OUTPUT);
    digitalWrite(sensor.controlPin, LOW);
    delay(10);
  }

  // Change addresses of the sensors by waking one after another starting from zero
  for(int i = 0; i < DISTANCE_SENSORS_COUNT; i++) {
    Debug.print("Init sensor ");
    Debug.println(i + 1);
    digitalWrite(DISTANCE_SENSORS_CONTROL_PINS[i], HIGH);
    delay(10);
    if (!sensors[i]->init()) {
      Debug.print("Failed init sensor ");
      Debug.println(i + 1);
      while(1);
    }
    sensors[i]->setAddress(DISTANCE_SENSORS_ADDRESS_START_ADDRESS + i);
    sensors[i]->setMeasurementTimingBudget(20000);
    delay(10);
  }

#endif
}

void DistanceSensors::refresh() {
#ifdef USE_DISTANCE_SENSORS
  static unsigned int readingIndex;
  for(int i = 0; i < DISTANCE_SENSORS_COUNT; i++) {
    unsigned int reading = sensors[i]->readRangeSingleMillimeters();
    if(!sensors[i]->timeoutOccurred()) {
      readings[i][readingIndex] = reading >= MAX_DISTANCE ? MAX_DISTANCE : reading;
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
  for(auto & reading : readings) {
    unsigned int sum = 0;
    for(unsigned int j : reading) {
      sum += j;
    }
    unsigned int curReading = sum / DISTANCE_AVG_WINDOW_SIZE;
    if (curReading < minReading) {
      minReading = curReading;
    }
  }
#endif
  return minReading;
}

int DistanceSensors::getDistDiff() {
  int leftMostDistance = 0, rightMostDistance = 0;
#ifdef USE_DISTANCE_SENSORS
  unsigned int leftMostSum = 0 , rightMostSum = 0;
  for(int j = 0; j < DISTANCE_AVG_WINDOW_SIZE; j++) {
    leftMostSum += readings[0][j];
    rightMostSum += readings[DISTANCE_SENSORS_COUNT - 1][j];
  }
  leftMostDistance = leftMostSum / DISTANCE_AVG_WINDOW_SIZE;
  rightMostDistance = rightMostSum / DISTANCE_AVG_WINDOW_SIZE;
#endif
  return leftMostDistance - rightMostDistance;
}
