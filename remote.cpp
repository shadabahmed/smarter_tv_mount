#include "Arduino.h"
#include "remote.h"

Remote::Remote() {
  pinMode(BUTTON_D0, INPUT);
  pinMode(BUTTON_D1, INPUT);
  pinMode(BUTTON_D2, INPUT);
  pinMode(BUTTON_D3, INPUT);
}

bool Remote::isButtonPressed(){ 
  return this->getButtonCode();
}

int Remote::getButtonCode(){ 
  return digitalRead(BUTTON_D0) | (digitalRead(BUTTON_D1) << 1) | (digitalRead(BUTTON_D2) << 2) | (digitalRead(BUTTON_D3) << 3);
}