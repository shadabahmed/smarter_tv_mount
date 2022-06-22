#ifndef MOTOR_COTROLLER_H

#define MOTOR_COTROLLER_H

#define CURRENT_READINGS_WINDOW_SIZE 50

class MotorController
{
  public:
    MotorController(int, int, int, int, int);
    void setup();
    void run(int, int = 80);
    void stop();
    int getCurrent();
    void callback();
    void resetFault();
    bool isFaultDetected();
    void monitor();
  private:
    void setFault();
    int selA;
    int inA;
    int inB;
    int pwm;
    int currentSense;
    int currentReadings[CURRENT_READINGS_WINDOW_SIZE];
    bool started;
    bool faultDetected;
    int maxCurrent;
    int avgCurrent;;
    void resetCurrentReadings();
};

#endif