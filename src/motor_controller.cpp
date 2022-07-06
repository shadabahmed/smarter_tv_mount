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

void MotorController::begin() const {
  pinMode(selA, OUTPUT);
  pinMode(inA, OUTPUT);
  pinMode(inB, OUTPUT);
  pinMode(pwm, OUTPUT);
  digitalWrite(selA, LOW);
  digitalWrite(inA, LOW);
  digitalWrite(inB, LOW);
  digitalWrite(pwm, LOW);
}

void MotorController::run(int dir, int dutyCycle) const {
  if (dir) {
    digitalWrite(selA, HIGH);
    digitalWrite(inA, HIGH);
    digitalWrite(inB, LOW);
  } else {
    digitalWrite(selA, LOW);
    digitalWrite(inA, LOW);
    digitalWrite(inB, HIGH);
  }
  analogWrite(pwm, map(dutyCycle, 0, 100, 0, 255));
}

void MotorController::stop() {
  analogWrite(pwm, 0);
  this->resetCurrentReadings();
}

void MotorController::refresh() {
#ifdef ARDUINO_AVR_NANO_EVERY
  currentReadings[++currentReadingsIndex] = analogRead(currentSense);
#elif ARDUINO_NANO_RP2040_CONNECT
  currentReadings[++currentReadingsIndex] = analogRead(NinaPin(currentSense));
#endif
  if (currentReadingsIndex == CURRENT_READINGS_WINDOW_SIZE)
    currentReadingsIndex = 0;
}

int MotorController::getCurrent() {
  int cumulatedCurrent = 0;
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


