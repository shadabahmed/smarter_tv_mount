#include <debug.h>
#include <mount_state_machine.h>

MountStateMachine mount;
void setup(){
  Debug::setup();
  Debug::println("Init...");
  mount.setup();
  Debug::println("Finished init !");
  delay(SETUP_WAIT);
  Debug::clear();
}

void loop(){
  MountStateMachine::Event event = mount.getEvent();
  mount.printInfo(event);
  mount.updateState(event);
  delay(TICK);
}
