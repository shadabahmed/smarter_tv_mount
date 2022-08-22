#include "Arduino.h"
#include "motor_controller.h"

void MotorController::begin() {
  resetCurrentReadings();
  pinMode(selA, OUTPUT);
  pinMode(inA, OUTPUT);
  pinMode(inB, OUTPUT);
  pinMode(pwm, OUTPUT);
  digitalWrite(selA, LOW);
  digitalWrite(inA, LOW);
  digitalWrite(inB, LOW);
  digitalWrite(pwm, LOW);
#ifdef ARDUINO_AVR_NANO_EVERY
  // Hack to increase PWM frequency. Instead of counting all the way upto 255, we just count upto MAX_PWM_COUNTER_VAL.
  // PWM_FREQ = 16,000,000 / 64 * MAX_PWM_COUNTER_VAL;
  TCB0.CCMPL = MAX_PWM_COUNTER_VAL;
  TCB0.CCMPH = MAX_PWM_COUNTER_VAL / 2;

  TCB1.CCMPL = MAX_PWM_COUNTER_VAL;
  TCB1.CCMPH = MAX_PWM_COUNTER_VAL / 2;
#endif
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
  analogWrite(pwm, map(dutyCycle, 0, 100, 0, MAX_PWM_COUNTER_VAL));
}

void MotorController::stop() {
  analogWrite(pwm, 0);
  resetCurrentReadings();
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


