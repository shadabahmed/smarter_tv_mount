#include <debug.h>
#include <mount_state_machine.h>
#include <avr/wdt.h>

#ifdef MILLIS_USE_TIMERA0
#error "This sketch takes over TCA0 - please use a different timer for millis"
#endif

MountStateMachine mount;
void setup(){
  // Needed for high frequency PWM at Pin 5. Sets prescalar for TCA0 to clock freq
  TCA0.SPLIT.CTRLA = TCB_ENABLE_bm;

  Debug::setup();
  Debug::println("Init...");
  mount.setup();
  Debug::println("Finished init !");
  delay(SETUP_WAIT);
  Debug::clear();
  // Enable watch dog timer with 2s interval
  wdt_enable(WDTO_2S);
}

void loop(){
  wdt_reset();
  MountStateMachine::Event event = mount.getEvent();
  mount.printInfo(event);
  mount.transitionState(event);
  delay(TICK);
}
