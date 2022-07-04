
#include <debug.h>
#include <mount_state_machine.h>
#ifdef ARDUINO_AVR_NANO_EVERY
#include <AVR/wdt.h>
#elif ARDUINO_NANO_RP2040_CONNECT
extern "C" {
#include <hardware/watchdog.h>
};
#else
#error "Error ! Board not supported !"
#endif

MountStateMachine mount;
void setup() {
#ifdef ARDUINO_AVR_NANO_EVERY
  // Needed for high frequency PWM at Pin 5. Sets pre-scalar for TCA0 to clock freq
  TCA0.SPLIT.CTRLA = TCB_ENABLE_bm;
#endif
  Debug.begin();
  Debug.println("Init...");
  mount.begin();
  Debug.println("Finished init !");
  delay(SETUP_DELAY);
  Debug.clear();
  // Enable watch dog timer with 2s interval
#ifdef ARDUINO_AVR_NANO_EVERY
  wdt_enable(WDTO_2S);
#elif ARDUINO_NANO_RP2040_CONNECT
  watchdog_enable(2000, true);
#endif
}

void loop() {
#ifdef ARDUINO_AVR_NANO_EVERY
  wdt_reset();
#elif ARDUINO_NANO_RP2040_CONNECT
  watchdog_update();
#endif
  mount.update();
  delay(LOOP_DELAY);
}
