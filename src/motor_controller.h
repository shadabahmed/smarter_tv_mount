#ifndef MOTOR_CONTROLLER_H_
#define MOTOR_CONTROLLER_H_
#include "debug.h"

#ifdef ARDUINO_AVR_NANO_EVERY
#define MAX_PWM_COUNTER_VAL 4
#elif ARDUINO_NANO_RP2040_CONNECT
#define MAX_PWM_COUNTER_VAL 255
#include "WiFiNINA.h"
#endif

#define CURRENT_READINGS_WINDOW_SIZE 30

class MotorController
{
  public:
    MotorController(int, int, int, int, int);
    void begin() const ;
    void refresh();
    void run(int, int = 80) const;
    void stop();
    int getCurrent();
  private:
    int selA;
    int inA;
    int inB;
    int pwm;
    int currentSense;
    int * currentReadings;
    int currentReadingsIndex = 0;
    void resetCurrentReadings();
};

#endif