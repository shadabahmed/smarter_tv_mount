//
// Created by Shadab Ahmed on 7/1/22.
//
#include "Arduino.h"
#include "debug.h"
#include "distance_sensors.h"

DistanceSensors::DistanceSensors() {
  for(int i = 0; i < DISTANCE_SENSORS_COUNT; i++) {
#ifdef USE_DISTANCE_SENSORS
    sensors[i] = new Adafruit_VL53L0X;
    pinMode(DISTANCE_SENSORS_CONTROL_PINS[i], OUTPUT);
#endif
    for(int j = 0; j < DISTANCE_AVG_WINDOW_SIZE; j++) {
      readings[i][j] = MAX_DISTANCE;
    }
  }
}

void DistanceSensors::begin() {
#ifdef USE_DISTANCE_SENSORS
  // Set all sensors XSHUT to low
  for(int i = 0; i < DISTANCE_SENSORS_COUNT; i++) {
    digitalWrite(DISTANCE_SENSORS_CONTROL_PINS[i], 0);
  }
  delay(10);

  // Set all sensors XSHUT to high
  for(int i = 0; i < DISTANCE_SENSORS_COUNT; i++) {
    digitalWrite(DISTANCE_SENSORS_CONTROL_PINS[i], 1);
  }
  delay(10);

  // Set all sensors XSHUT to low except first
  for(int i = 1; i < DISTANCE_SENSORS_COUNT; i++) {
    digitalWrite(DISTANCE_SENSORS_CONTROL_PINS[i], 0);
  }
  delay(10);

  // Change addresses of the sensors by waking one after another starting from zero
  for(int i = 0; i < DISTANCE_SENSORS_COUNT; i++) {
    Debug::print("Init sensor ");
    Debug::println(i);
    digitalWrite(DISTANCE_SENSORS_CONTROL_PINS[i], 1);
    if(!sensors[i]->begin(DISTANCE_SENSORS_ADDRESS_START_ADDRESS + i)) {
      Debug::print("Failed init sensor ");
      Debug::println(i);
      while(1);
    }
    delay(10);
  }

#endif
}

void DistanceSensors::refresh() {
#ifdef USE_DISTANCE_SENSORS
  static unsigned int readingIndex;
  for(int i = 0; i < DISTANCE_SENSORS_COUNT; i++) {
    VL53L0X_RangingMeasurementData_t measure;
    sensors[i]->rangingTest(&measure, false);
    if(measure.RangeStatus != 4) {
      readings[i][readingIndex] = measure.RangeMilliMeter >= MAX_DISTANCE ? MAX_DISTANCE : measure.RangeMilliMeter;
    }
  }
  readingIndex++;
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
  int leftMostDistance = 0;
  int rightMostDistance = 0;
#ifdef USE_DISTANCE_SENSORS
  int leftMostSum = 0 , rightMostSum = 0;
  for(int j = 0; j < DISTANCE_AVG_WINDOW_SIZE; j++) {
    leftMostSum += readings[0][j];
    rightMostSum += readings[DISTANCE_AVG_WINDOW_SIZE - 1][j];
  }
  leftMostDistance = leftMostSum / DISTANCE_AVG_WINDOW_SIZE;
  rightMostDistance = rightMostSum / DISTANCE_AVG_WINDOW_SIZE;
#endif
  return leftMostDistance - rightMostDistance;
}
