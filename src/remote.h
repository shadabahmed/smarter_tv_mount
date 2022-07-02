#ifndef REMOTE_H_
#include "Arduino.h"
#include "debug.h"
#define REMOTE_H_

#define BUTTON_D0 10
#define BUTTON_D1 11
#define BUTTON_D2 12
#define BUTTON_D3 16

class Remote {
  public:
  Remote() = default;
  void begin() {
    Debug::println("Init remote...");
    pinMode(BUTTON_D0, INPUT);
    pinMode(BUTTON_D1, INPUT);
    pinMode(BUTTON_D2, INPUT);
    pinMode(BUTTON_D3, INPUT);
  };
  bool isButtonPressed() {
    return getButtonCode() > 0;
  }
  int getButtonCode() {
    return digitalRead(BUTTON_D0) |
    (digitalRead(BUTTON_D1) << 1) |
    (digitalRead(BUTTON_D2) << 2) |
    (digitalRead(BUTTON_D3) << 3);
  }
};
#endif
