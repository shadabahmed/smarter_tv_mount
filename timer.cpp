#include "Arduino.h"
#include "timer.h"

Timer::Timer() {
  callbackTargets = new TimerCallbackTarget*[4];
}

Timer Timer::getInstance() 
{
    static Timer instance;
    return instance;
}

void Timer::start(){
  TCB1.CTRLB = TCB_CNTMODE_INT_gc;  
  TCB1.CNT = 0x0;
  TCB1.CCMP = 0xFFFE;
  TCB1.EVCTRL = 0;
  TCB1.INTCTRL = TCB_CAPT_bm; // enable isr  
  TCB1.CTRLA = (TCB_RUNSTDBY_bm) | (TCB_CLKSEL_CLKDIV2_gc) | (TCB_ENABLE_bm);  
}

void Timer::attachCallbackTarget(TimerCallbackTarget * callbackTarget) {
  static int targetCount = 0;
  if (targetCount < 4) {
    callbackTargets[++targetCount] = callbackTarget;
  }  
}

ISR(TCB1_INT_vect){
  if(TCB1.INTFLAGS & TCB_CAPT_bm)
  {
    // unsigned int outputPin = LED_BUILTIN;
    // static unsigned int counter = 0;
    // if(++counter == 10){
    //   static bool toggle1 = false;
    //   static bool started = false;
    //   if (!started)
    //   {
    //     started = true;
    //     pinMode(outputPin, OUTPUT);
    //   }
    //   digitalWrite(outputPin, toggle1);
    //   toggle1 = !toggle1;
    //   counter = 0;
    // }
    TCB1.CNT = 0x0;
    TCB1.INTFLAGS = TCB_CAPT_bm;
    for(int i =0; i < 4; i++) {
      if (Timer::getInstance().callbackTargets[i]) {
        Timer::getInstance().callbackTargets[i]->callback();
      }      
    }
  }
}