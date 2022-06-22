
#ifndef TIMER_H

#define TIMER_H

class TimerCallbackTarget {
  public:
  virtual void callback() = 0;
};

class Timer {
  public:
  static Timer getInstance();
  void start();
  void attachCallbackTarget(TimerCallbackTarget *);
  TimerCallbackTarget** callbackTargets;
  private:
  Timer();
  static Timer * instance;
};

#endif