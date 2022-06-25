#ifndef MOTOR_CONTROLLER_H_
#define MOTOR_CONTROLLER_H_
#include "debug.h"

#define CURRENT_READINGS_WINDOW_SIZE 30

class MotorController
{
  public:
    MotorController(int, int, int, int, int);
    void setup() ;
    void run(int, int = 80);
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