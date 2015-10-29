#include <Wire.h>
#include <CapacitiveSensor.h>
#include <Piccolino_OLED.h>
/*
 * CapitiveSense Library Demo Sketch
 * Paul Badger 2008
 * Uses a high value resistor e.g. 10M between send pin and receive pin
 * Resistor effects sensitivity, experiment with values, 50K - 50M. Larger resistor values yield larger sensor values.
 * Receive pin is the sensor pin - try different amounts of foil/metal on this pin
 * Modified for Piccolino bezel-touch by Alex Sardo March 2015
 */
 
Piccolino_OLED     screen;
CapacitiveSensor   tleft = CapacitiveSensor(2,3);
CapacitiveSensor   tright = CapacitiveSensor(2,4);

#define TRIGGER_LEVEL 50 // change as appropriate

void setup()                    
{
   Serial.begin(9600);
   screen.begin();
   delay(100);
   Serial.print(F("Go ahead, touch my face-plate...\n"));
   screen.setTextSize(3);
}

void loop()                    
{
    long start = millis();
    long t1 =  tleft.capacitiveSensor(30)/10;
    long t2 =  tright.capacitiveSensor(30)/10;

    screen.clear();
    
    if(t1>TRIGGER_LEVEL&&t2>TRIGGER_LEVEL) {
      Serial.print(F("BOTH\n"));
      screen.setCursor(30,20);
      screen.print(F("BOTH"));
    } else {
      if(t1>TRIGGER_LEVEL) {
        Serial.print(F("LEFT\n"));
        screen.setCursor(0,20);
        screen.print(F("LEFT"));
      }
      if(t2>TRIGGER_LEVEL) {
        Serial.print(F("RIGHT\n"));
        screen.setCursor(40,20);
        screen.print(F("RIGHT"));
      }
    }
 
    screen.update();
}
