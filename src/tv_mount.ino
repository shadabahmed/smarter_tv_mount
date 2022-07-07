
#include <debug.h>
#include <mount_state_machine.h>
#define ENABLE_WDT
#ifdef ENABLE_WDT
#ifdef ARDUINO_AVR_NANO_EVERY
#include <AVR/wdt.h>
#elif ARDUINO_NANO_RP2040_CONNECT
extern "C" {
#include <hardware/watchdog.h>
};
#else
#error "Error ! Board not supported !"
#endif
#endif

#define SETUP_DELAY 2000
#define LOOP_DELAY 2

MountStateMachine mount;
void setup() {
  Wire.begin();
  Wire.setClock(400000L);
  Debug.begin();
  Debug.println("Init...");
  mount.begin();
  Debug.println("Finished init !");
  delay(SETUP_DELAY);
  Debug.clear();
#ifdef ENABLE_WDT
  // Enable watch dog timer with 2s interval
#ifdef ARDUINO_AVR_NANO_EVERY
  wdt_enable(WDTO_2S);
#elif ARDUINO_NANO_RP2040_CONNECT
  watchdog_enable(2000, true);
#endif
#endif
}

void loop() {
#ifdef ENABLE_WDT
#ifdef ARDUINO_AVR_NANO_EVERY
  wdt_reset();
#elif ARDUINO_NANO_RP2040_CONNECT
  watchdog_update();
#endif
#endif
  mount.update();
  delay(LOOP_DELAY);
}
