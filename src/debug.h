#include "Arduino.h"

#ifndef DEBUG_H_

#define DEBUG_H_
#define SERIAL_BAUD 115200
#define SERIAL_DEBUG
#define OLED_DEBUG

#ifdef OLED_DEBUG
#include <Wire.h>
#define OPTIMIZE_I2C 1
#include "SSD1306Ascii.h"
#include "SSD1306AsciiWire.h"
#endif

class DebugClass {
public:
#ifdef OLED_DEBUG
#ifdef ARDUINO_AVR_NANO_EVERY
  DebugClass() : display(SSD1306AsciiWire()) {};
#elif ARDUINO_NANO_RP2040_CONNECT
  DebugClass() : display(SSD1306AsciiWire(Wire)) {};
#endif
#else
  DebugClass() {};
#endif

  void begin() {
    Serial.begin(SERIAL_BAUD);
#ifdef OLED_DEBUG
    display.begin(&Adafruit128x64, 0x3C);
    display.setFont(Stang5x7);
    display.clear();
#endif
  }

  void clear() {
#ifdef SERIAL_DEBUG
    Serial.print("\033[2J"  );
#endif
#ifdef OLED_DEBUG
    display.clear();
    display.home();
#endif
  }

  void home() {
#ifdef OLED_DEBUG
    display.home();
#endif
  }

  void print(const char * str) {
#ifdef SERIAL_DEBUG
    Serial.print(str);
#endif
#ifdef OLED_DEBUG
  display.print(str);
#endif
  }

  void print(const arduino::__FlashStringHelper * str) {
#ifdef SERIAL_DEBUG
    Serial.print(str);
#endif
#ifdef OLED_DEBUG
    display.print(str);
#endif
  }

  void print(int val) {
#ifdef SERIAL_DEBUG
    Serial.print(val);
#endif
#ifdef OLED_DEBUG
    display.print(val);
#endif
  }

  void println(const char * str) {
#ifdef SERIAL_DEBUG
    Serial.println(str);
#endif
#ifdef OLED_DEBUG
    display.println(str);
    display.clearToEOL();
#endif
  }

  void println(const arduino::__FlashStringHelper * str) {
#ifdef SERIAL_DEBUG
    Serial.println(str);
#endif
#ifdef OLED_DEBUG
    display.println(str);
    display.clearToEOL();
#endif
  }

  void println(int val) {
#ifdef SERIAL_DEBUG
    Serial.println(val);
#endif
#ifdef OLED_DEBUG
    display.println(val);
    display.clearToEOL();
#endif
  }
#ifdef OLED_DEBUG
  SSD1306AsciiWire display;
#endif
};

extern DebugClass Debug;
#endif

