#include "Arduino.h"
#include "motor_controller.h"

MotorController::MotorController(int selA, int inA, int inB, int pwm, int currentSense) {
  this->selA = selA;
  this->inA = inA;
  this->inB = inB;
  this->pwm = pwm;
  this->currentSense = currentSense;
  this->currentReadings = new int[CURRENT_READINGS_WINDOW_SIZE];
  resetCurrentReadings();
}

void MotorController::setup() {
  pinMode(selA, OUTPUT);
  pinMode(inA, OUTPUT);
  pinMode(inB, OUTPUT);
  pinMode(pwm, OUTPUT);
  pinMode(currentSense, INPUT_PULLDOWN);
  digitalWrite(selA, 0);
  digitalWrite(inA, 0);
  digitalWrite(inB, 0);
  digitalWrite(pwm, 0);
}

void MotorController::run(int dir, int dutyCycle) {
  if (dir) {
    digitalWrite(selA, 1);
    digitalWrite(inA, 1);
    digitalWrite(inB, 0);
  } else {
    digitalWrite(selA, 0);
    digitalWrite(inA, 0);
    digitalWrite(inB, 1);
  }
  analogWrite(pwm, map(dutyCycle, 0, 100, 0, 255));
}

void MotorController::stop() {
  digitalWrite(pwm, 0);
  this->resetCurrentReadings();
}

int MotorController::getCurrent() {
  int cumulatedCurrent = 0;
  currentReadings[++currentReadingsIndex] = analogRead(currentSense);
  if (currentReadingsIndex == CURRENT_READINGS_WINDOW_SIZE)
    currentReadingsIndex = 0;
  for (int i = 0; i < CURRENT_READINGS_WINDOW_SIZE; i++) {
		cumulatedCurrent += currentReadings[i];
	}
  return cumulatedCurrent / CURRENT_READINGS_WINDOW_SIZE;
}

void MotorController::resetCurrentReadings() {
  // Reset the current readings since motor is stopped
  for (int i = 0; i < CURRENT_READINGS_WINDOW_SIZE; i++) {
    currentReadings[i] = 0;
  }
}


