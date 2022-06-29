#include <debug.h>
#include <mount_state_machine.h>
#include <avr/wdt.h>

MountStateMachine mount;
void setup(){
  Debug::setup();
  Debug::println("Init...");
  mount.setup();
  Debug::println("Finished init !");
  delay(SETUP_WAIT);
  Debug::clear();
  wdt_enable(WDTO_2S);
}

void loop(){
  wdt_reset();
  MountStateMachine::Event event = mount.getEvent();
  mount.printInfo(event);
  mount.transitionState(event);
  delay(TICK);
}
