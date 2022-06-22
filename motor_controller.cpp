#include "Arduino.h"
#include "motor_controller.h"
#include "timer.h"

MotorController::MotorController(int _selA, int _inA, int _inB, int _pwm, int _currentSense) {
  selA = _selA;
  inA = _inA;
  inB = _inB;
  pwm = _pwm;
  currentSense = _currentSense;
  started = false;
  faultDetected = false;
  avgCurrent = 0;  
  this->resetCurrentReadings();  
}

void MotorController::setup() {
  pinMode(selA, OUTPUT);
  pinMode(inA, OUTPUT);
  pinMode(inB, OUTPUT);
  pinMode(pwm, OUTPUT);
  digitalWrite(selA, 0);
  digitalWrite(inA, 0);
  digitalWrite(inB, 0);
  digitalWrite(pwm, 0);  
}

void MotorController::run(int dir, int dutyCycle)
{ 
  // Do not run if fault detected
  if(this->faultDetected)
    return;

  if (dir) {
    digitalWrite(selA, 1);
    digitalWrite(inA, 1);
    digitalWrite(inB, 0);
    this->maxCurrent = 400;
  } else {
    digitalWrite(selA, 0);
    digitalWrite(inA, 0);
    digitalWrite(inB, 1);
    this->maxCurrent = 100;
  }
  analogWrite(pwm, float(dutyCycle) / 100 * 1024);
  this->started = true;
}

void MotorController::stop()
{ 
  digitalWrite(pwm, 0);
  this->started = false;
  this->resetCurrentReadings();
}

int MotorController::getCurrent()
{ 
  int cumulatedCurrent = 0;
  static int currentReadingsIndex = 0;
  currentReadings[++currentReadingsIndex] = analogRead(currentSense);
  if (currentReadingsIndex == CURRENT_READINGS_WINDOW_SIZE)
    currentReadingsIndex = 0;  
  for(byte i=0; i < CURRENT_READINGS_WINDOW_SIZE; i++)
	{
		cumulatedCurrent += currentReadings[i];
	}
  return cumulatedCurrent / CURRENT_READINGS_WINDOW_SIZE;
}

void MotorController::resetCurrentReadings() {
  // Reset the current readings since motor is stopped
  for(byte i=0; i < CURRENT_READINGS_WINDOW_SIZE; i++)
  {
    currentReadings[i] = 0;
  }
}

void MotorController::setFault()
{ 
  this->faultDetected = true;
}

bool MotorController::isFaultDetected()
{ 
  return this->faultDetected;
}

void MotorController::resetFault()
{ 
  this->faultDetected = false;
  digitalWrite(LED_BUILTIN, 0);
}

void MotorController::monitor(){
  // If spike in current, then stop the motor
  this->faultDetected = this->started && this->getCurrent() > maxCurrent;     
}
  
