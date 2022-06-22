#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include "mount_controller.h"

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels
#define IR_RECEIVE_PIN 12

// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
#define OLED_RESET     -1 // Reset pin # (or -1 if sharing Arduino reset pin)
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
MountController mount;
//AS5600 as5600;

void setup(){
  Serial.begin(115200);
  mount.setup();
  // pinMode(2, INPUT);
  // pinMode(3, INPUT); 
  // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { 
    Serial.println(F("SSD1306 allocation failed"));
    for(;;); // Don't proceed, loop forever
  }
  display.setTextSize(1.5);
  display.setTextColor(SSD1306_WHITE);
  display.display();
  // text size
  // as5600.begin(4);     //  set the direction pin
  // as5600.setDirection(AS5600_CLOCK_WISE);
  pinMode(21, INPUT_PULLUP);
  delay(100);  
}

void loop(){
  display.clearDisplay();
  display.setCursor(0, 0); 
  display.print("State: ");
  display.println(mount.getStateString(mount.getState()));
  display.print("Event: ");
  display.println(mount.getEventString(mount.getEvent()));
  display.display(); 
  //   mount.monitor();
  //   printStatus();    
  //   if(mount.isFaultDetected()) {
  //     mount.stop();
  //     delay(2000);
  //     mount.resetFault();
  //   } else {        
  //     display.clearDisplay();
  //     display.setCursor(0, 0); 
  //     display.print("Code: ");
  //     display.println(remote.getButtonCode());
  //     display.display();
  //    if (remote.isButtonPressed()) {
  //      Serial.println(remote.getButtonCode());
  //      if (remote.getButtonCode() == 8) {
  //       mount.moveUp();
  //      } else if (remote.getButtonCode() == 4) {
  //       mount.moveDown();
  //      } else {
  //       //motor.stop();
  //       mount.stop();
  //      }
  //   }      
  // }
  // // display.clearDisplay();
  // // display.setCursor(0, 0);
  // // mount.monitor();
  delay(100);
}

void printStatus() {
  Serial.print("\033[2J"  );
  Serial.print("A0 is : ");
  Serial.println(analogRead(A0));
  Serial.print("A1 is : ");
  Serial.println(analogRead(A1));
  Serial.print("USB is : ");
  Serial.println(digitalRead(21) == 0 ? "connected !" : "not connected !");
  Serial.print("Motor Current is : ");
  Serial.println(mount.getUpDownMotorCurrent());
}

