#include "Arduino.h"
#include "debug.h"
#include "distance_sensors.h"

DistanceSensors::DistanceSensors() {
#ifdef USE_DISTANCE_SENSORS
  for(int i = 0; i < DISTANCE_SENSORS_COUNT; i++) {
    sensors[i] = new VL53L0X;
    for(int j = 0; j < DISTANCE_AVG_WINDOW_SIZE; j++) {
      readings[i][j] = MAX_DISTANCE;
    }
  }
#endif
}

void DistanceSensors::begin() {
#ifdef USE_DISTANCE_SENSORS
  for(int i : DISTANCE_SENSORS_CONTROL_PINS) {
    pinMode(i, OUTPUT);
    digitalWrite(i, 0);
    delay(10);
  }

  // Change addresses of the sensors by waking one after another starting from zero
  for(int i = 0; i < DISTANCE_SENSORS_COUNT; i++) {
    Debug.print("Init sensor ");
    Debug.println(i + 1);
    digitalWrite(DISTANCE_SENSORS_CONTROL_PINS[i], 1);
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
#else
#endif
}

int DistanceSensors::getMinDistance() {
  int minReading = MAX_DISTANCE;
#ifdef USE_DISTANCE_SENSORS
  for(int i = 0; i < DISTANCE_SENSORS_COUNT; i++) {
    int sum = 0;
    for(int j = 0; j < DISTANCE_AVG_WINDOW_SIZE; j++) {
      sum += readings[i][j];
    }
    if (sum / DISTANCE_AVG_WINDOW_SIZE < minReading) {
      minReading = sum / DISTANCE_AVG_WINDOW_SIZE;
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
