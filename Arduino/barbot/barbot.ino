#include <Wire.h>
#include <SPI.h>
#include <Piccolino_RAM.h>
#include <Piccolino_OLED_SRAM.h>

#include "HX711.h"
#include "PCF8575.h"

HX711 scale(3, 4);
PCF8575 expander;
Piccolino_OLED_SRAM display;
const byte expanderAddress = 0x20;
const byte s7sAddress = 0x71;

bool ledEnabled = false;

void setup() {
  Serial.begin(57600);
  Serial.println("*-*-*-*-*-*-*-*-*-*-*-*-*");
  Serial.println("*-*-*-*-*-*-*-*-*-*-*-*-*");
  Serial.println("BarBot");
  Serial.println("*-*-*-*-*-*-*-*-*-*-*-*-*");
  Serial.println("*-*-*-*-*-*-*-*-*-*-*-*-*");

  Serial.println("Setting up OLED display");
  display.begin();
  display.clear();

  display.setCursor(12,10);
  display.setTextColor(WHITE);
  display.setTextSize(3);
  display.print("BarBot");
  display.update();

  Wire.begin();

  if(ledEnabled) {
    Serial.println("Setting up LED display");
    clearDisplayI2C();
    setBrightnessI2C(0x255);
    setDecimalsI2C(0b00001111);
    delay(250);
    setDecimalsI2C(0b00000000);
    lcdBarBotAnimation();
  }
  
  Serial.println("Setting up pumps");
  expander.begin(expanderAddress);
  expander.pinMode(0, OUTPUT);
  expander.pinMode(1, OUTPUT);

  /*
  Serial.println("Before setting up the scale:");
  Serial.print("read: \t\t");
  Serial.println(scale.read());      // print a raw reading from the ADC

  Serial.print("read average: \t\t");
  Serial.println(scale.read_average(20));   // print the average of 20 readings from the ADC

  Serial.print("get value: \t\t");
  Serial.println(scale.get_value(5));   // print the average of 5 readings from the ADC minus the tare weight (not set yet)

  Serial.print("get units: \t\t");
  Serial.println(scale.get_units(5), 1);  // print the average of 5 readings from the ADC minus tare weight (not set) divided 
            // by the SCALE parameter (not set yet)
  */

  Serial.println("Calibrating scale");
  // this value is obtained by calibrating the scale with known weights
  scale.set_scale(-54757.57f);                      
  // reset the scale to 0
  // scale.tare();

  /*
  Serial.println("After setting up the scale:");

  Serial.print("read: \t\t");
  Serial.println(scale.read());                 // print a raw reading from the ADC

  Serial.print("read average: \t\t");
  Serial.println(scale.read_average(20));       // print the average of 20 readings from the ADC

  Serial.print("get value: \t\t");
  Serial.println(scale.get_value(5));   // print the average of 5 readings from the ADC minus the tare weight, set with tare()

  Serial.print("get units: \t\t");
  Serial.println(scale.get_units(5), 1);        // print the average of 5 readings from the ADC minus tare weight, divided 
            // by the SCALE parameter set with set_scale
  */
  
  Serial.println("Ready.");
  Serial.println("Enter serial command");
  Serial.println("Format: {pump number} {weight in milligrams} {name of liquor}");
  Serial.println("eg. 01 0084 3/4oz Bourbon");
  Serial.println("==========================");

  display.setCursor(20,50);
  display.setTextColor(WHITE);
  display.setTextSize(1);
  display.print("at your service");
  display.update();  
}

enum state {
  waiting,
  tare,
  dispensing
};

#define MAX_LINE 50

state currentState;
char serialInput[MAX_LINE+1];
double currentWeight;
int percentWeight;
int barHeight;
char ledString[10];
String displayWeight;
char oledPercentString[10];
char oledWeightString[10];

int pumpNumber;
double targetWeight;
String liquorName;

void loop() {

  if(ledEnabled) {
    // Read the scale
    currentWeight = scale.get_units(1);
  
    // for some reason this started crashing the arduino... the thing might be broken tho
    if(currentWeight < 0) {
      s7sSendStringI2C("   -");
    } else {
      displayWeight = String(currentWeight);
      displayWeight.replace(".","");
      s7sSendStringI2C(displayWeight);
      if(currentWeight >= 100) { 
        setDecimalsI2C(0b00000100);
      } else if(currentWeight >= 10) {
        setDecimalsI2C(0b00000010);
      } else if(currentWeight >= 0) {
        setDecimalsI2C(0b00000001);
      }
    }
  }
  
  switch(currentState) {
    case waiting:

      while(!lineAvailable(MAX_LINE, serialInput)) {
        delay(10);
      }
    
      //if(readLine(MAX_LINE, serialInput)) {
        Serial.println(serialInput);
        if(!parseSerialCommand((String)serialInput)) {
          Serial.println("Invalid serial command");
          Serial.println("Format: {pump number} {weight in milliigrams} {name of liquor}");
          Serial.println("eg. 01 0080 3/4oz Bourbon");
          return;
        }
        Serial.print("Pump: ");
        Serial.print(pumpNumber);
        Serial.print(" Weight: ");
        Serial.print(targetWeight);
        Serial.print(" Name: ");
        Serial.println(liquorName);
  
        currentState = tare;
      //}
      break;
      
    case tare:
      if(ledEnabled) {
        setDecimalsI2C(0b00000000);
        s7sSendStringI2C("----");
      }
      
      Serial.println("Tare");

      display.clear();
      display.setTextColor(WHITE);
      display.setTextSize(1);
      display.setCursor(0,0);
      display.print(liquorName);
      display.setCursor(0,9);
      sprintf(ledString, "Pump %d", pumpNumber);
      display.print(ledString);
      display.update();

      scale.tare();

      currentState = dispensing;
      Serial.print("Dispensing now (");
      Serial.print(String(targetWeight, 3));
      Serial.println(")");
      
      // Turn on the pump
      expander.digitalWrite(pumpNumber-1, HIGH);
      break;
      
    case dispensing:
      // Read the scale
      if(!ledEnabled) {
        currentWeight = scale.get_units(2);
      }

      percentWeight = (int)(currentWeight / targetWeight * 100);
      if(percentWeight < 0) { 
        percentWeight = 0;
      }

      /*
      if(ledEnabled) {
        sprintf(ledString, "%2d%2d", pumpNumber, percentWeight);
        s7sSendStringI2C(ledString);
      }
      */

      barHeight = (int)(currentWeight / targetWeight * 64);
      if(barHeight < 0) {
        barHeight = 0;
      } else if(barHeight > 64) {
        barHeight = 64;
      }
      
      display.fillRect(128-10, 0, 10, 64-barHeight, BLACK);

      /*
      display.setTextColor(WHITE);
      display.setTextSize(1);
      display.setCursor(0,0);
      display.print(liquorName);
      */

      display.fillRect(128-10, 64-barHeight, 10, 64, WHITE);

      /*
      sprintf(oledPercentString, "%2d%%", percentWeight);
      display.setCursor(0,44);
      display.setTextColor(WHITE);
      display.print(oledPercentString);
      */

      sprintf(oledWeightString, "%01d.%03dg     ", (int)(currentWeight), abs((int)(currentWeight*1000)%1000));
      display.setCursor(0,52);
      display.print(oledWeightString);

      display.update();  

      // If the scale registers greater than the target weight,
      // stop the pump, and reset the state
      if(currentWeight >= targetWeight) {
        expander.digitalWrite(0, LOW);
        currentState = waiting;

        /*
        if(ledEnabled) {
          setDecimalsI2C(0b00000000);
          s7sSendStringI2C("DONE");
        }
        */

        display.setCursor(0,9);
        display.print("Done!         ");
        display.update();

        Serial.print("Done! Weight increased by ");
        Serial.println(currentWeight, 3);
      }
      break;

    default:
      break;
  }

  /*
  expander.digitalWrite(0, HIGH);
  expander.digitalWrite(1, LOW);
  
  Serial.println(scale.get_units(1), 2);
  //Serial.print("\t| average:\t");
  //Serial.println(scale.get_units(10), 1);
  //Serial.print("\t| raw:\t");
  //Serial.println(scale.read());

  //scale.power_down();              // put the ADC in sleep mode
  delay(100);
  //scale.power_up();
  //delay(100);
  expander.digitalWrite(0, LOW);
  expander.digitalWrite(1, HIGH);
  delay(100);
  */
}


// This custom function works somewhat like a serial.print.
//  You can send it an array of chars (string) and it'll print
//  the first 4 characters in the array.
void s7sSendStringI2C(String toSend)
{
  Wire.beginTransmission(s7sAddress);
  for (int i=0; i<4; i++)
  {
    Wire.write(toSend[i]);
  }
  Wire.endTransmission();
}

// Send the clear display command (0x76)
//  This will clear the display and reset the cursor
void clearDisplayI2C()
{
  Wire.beginTransmission(s7sAddress);
  Wire.write(0x76);  // Clear display command
  Wire.endTransmission();
}

// Set the displays brightness. Should receive byte with the value
//  to set the brightness to
//  dimmest------------->brightest
//     0--------127--------255
void setBrightnessI2C(byte value)
{
  Wire.beginTransmission(s7sAddress);
  Wire.write(0x7A);  // Set brightness command byte
  Wire.write(value);  // brightness data byte
  Wire.endTransmission();
}

// Turn on any, none, or all of the decimals.
//  The six lowest bits in the decimals parameter sets a decimal 
//  (or colon, or apostrophe) on or off. A 1 indicates on, 0 off.
//  [MSB] (X)(X)(Apos)(Colon)(Digit 4)(Digit 3)(Digit2)(Digit1)
void setDecimalsI2C(byte decimals)
{
  Wire.beginTransmission(s7sAddress);
  Wire.write(0x77);
  Wire.write(decimals);
  Wire.endTransmission();
}

boolean lineAvailable(int max_line, char *line)
{
     int c;
     static int line_idx = 0;
     boolean eol = false;
     if (max_line <= 0)    // handle bad values for max_line
     {
       eol = true;
       if (max_line == 0)
         line[0] = '\0';
     }
     else                // valid max_line
     {
       if (Serial.available() > 0)
       {
         c = Serial.read();
         if (c != -1)  // got a char -- should always be true
         {
           if (c == '\r')
             eol = true;
           else
             line[line_idx++] = c;
           if (line_idx >= max_line)
             eol = true;
           line[line_idx] = '\0';     // always terminate line, even if unfinished
           if (eol)
             line_idx = 0;           // reset for next line
           }
         }
       }
       return eol;
}

// When serial is available, read a full line up to max or \r
boolean readLine(int max_line, char *line)
{
  if(Serial.available() > 0) 
  {
    int c;
    int i = 0;
    bool eol = false;
    while(i <= max_line && !eol) {
      c = Serial.read();
      if(c == -1) { return false; }
      if(c == '\r') {
        eol = true;
      } else {
        line[i++] = c;
      }
    }
    line[i] = '\0';
    return true;
  } else {
    return false;
  }
}

boolean parseSerialCommand(String inputString)
{
  if(inputString.length() < 8) {
    return false;
  }
  
  pumpNumber = (int)inputString.substring(0,2).toInt();
  targetWeight = ((double)inputString.substring(3,7).toInt()) / 1000.0;
  liquorName = inputString.substring(8);

  if(pumpNumber == 0 || targetWeight == 0) {
    return false;
  }

  return true;
}

void defaultOLEDScreen()
{
  display.clear();
  display.setTextColor(WHITE);

  display.setCursor(12,10);
  display.setTextSize(3);
  display.print("BarBot");

  display.setCursor(20,50);
  display.setTextSize(1);
  display.print("at your service");
  display.update();  
}

void lcdBarBotAnimation()
{
  int delayTime = 150;
  s7sSendStringI2C("   b");
  delay(delayTime);
  s7sSendStringI2C("  ba");
  delay(delayTime);
  s7sSendStringI2C(" bar");
  delay(delayTime);
  s7sSendStringI2C("barb");
  delay(delayTime);
  s7sSendStringI2C("arbo");
  delay(delayTime);
  s7sSendStringI2C("rbot");
  delay(delayTime);
  s7sSendStringI2C("bot ");
  delay(delayTime);
  s7sSendStringI2C("ot  ");
  delay(delayTime);
  s7sSendStringI2C("t   ");
  delay(delayTime);
  s7sSendStringI2C("    ");
  delay(delayTime);
}

