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

class Debug {
public:
  DebugClass() {
  };
  static void setup() {
    Serial.begin(SERIAL_BAUD);
#ifdef OLED_DEBUG
    Wire.begin();
    Wire.setClock(400000L);
    display.begin(&Adafruit128x64, 0x3C);
    display.setFont(Adafruit5x7);
    display.clear();
#endif
  }

  static void clear() {
#ifdef SERIAL_DEBUG
    Serial.print("\033[2J"  );
#endif
#ifdef OLED_DEBUG
    display.clear();
    display.home();
#endif
  }

  static void home() {
#ifdef OLED_DEBUG
    display.home();
#endif
  }

  static void print(const char * str) {
#ifdef SERIAL_DEBUG
    Serial.print(str);
#endif
#ifdef OLED_DEBUG
  display.print(str);
#endif
  }

  static void print(const arduino::__FlashStringHelper * str) {
#ifdef SERIAL_DEBUG
    Serial.print(str);
#endif
#ifdef OLED_DEBUG
    display.print(str);
#endif
  }

  static void print(int val) {
#ifdef SERIAL_DEBUG
    Serial.print(val);
#endif
#ifdef OLED_DEBUG
    display.print(val);
#endif
  }

  static void println(const char * str) {
#ifdef SERIAL_DEBUG
    Serial.println(str);
#endif
#ifdef OLED_DEBUG
    display.println(str);
    display.clearToEOL();
#endif
  }

  static void println(const arduino::__FlashStringHelper * str) {
#ifdef SERIAL_DEBUG
    Serial.println(str);
#endif
#ifdef OLED_DEBUG
    display.println(str);
    display.clearToEOL();
#endif
  }

  static void println(int val) {
#ifdef SERIAL_DEBUG
    Serial.println(val);
#endif
#ifdef OLED_DEBUG
    display.println(val);
    display.clearToEOL();
#endif
  }
#ifdef OLED_DEBUG
private:
  inline static const SSD1306AsciiWire display;
#endif
};

#endif