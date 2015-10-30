#include <Wire.h>
#include <SPI.h>
#include <Piccolino_RAM.h>
#include <Piccolino_OLED_SRAM.h>
#include <HX711.h>
#include <PCF8575.h>

#define SCALE_DATA 3
#define SCALE_CLK 4

#define EXPPIN0 0
#define EXPPIN1 1
#define BAUD 57600
// this value is obtained by calibrating the scale with known weights
#define SCALE_CALIBRATION -1916

int buttonPins[2] = {14, 15};

HX711 scale(SCALE_DATA, SCALE_CLK);
PCF8575 expander;
Piccolino_OLED_SRAM display;
const byte expanderAddress = 0x20;
const byte s7sAddress = 0x71;

bool ledEnabled = false;

void setup() {
  Serial.begin(BAUD);
  Serial.println("*-*-*-*-*-*-*-*-*-*-*-*-*");
  Serial.println("*-*-*-*-*-*-*-*-*-*-*-*-*");
  Serial.println("BarBot");
  Serial.println("*-*-*-*-*-*-*-*-*-*-*-*-*");
  Serial.println("*-*-*-*-*-*-*-*-*-*-*-*-*");

  //Serial.println("Setting up OLED display");
  display.begin();
  display.clear();

  display.setCursor(12,10);
  display.setTextColor(WHITE);
  display.setTextSize(3);
  display.print("BarBot");
  display.update();

  Wire.begin();

  pinMode(buttonPins[0], INPUT);
  pinMode(buttonPins[1], INPUT);

  if(ledEnabled) {
    Serial.println("Setting up LED display");
    clearDisplayI2C();
    setBrightnessI2C(0x255);
    setDecimalsI2C(0b00001111);
    delay(250);
    setDecimalsI2C(0b00000000);
    lcdBarBotAnimation();
  }
  
  expander.begin(expanderAddress);
  expander.pinMode(EXPPIN0, OUTPUT);
  expander.pinMode(EXPPIN1, OUTPUT);

  scale.set_scale(SCALE_CALIBRATION);                      

  Serial.println("Ready.");
  Serial.println("Enter serial command");
  Serial.println("Format: {pump number} {weight in milligrams} {name of liquor}");
  Serial.println("eg. 01 00084 3/4oz Bourbon");
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

state currentState = waiting;
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

int lastButton1 = LOW;
int lastButton2 = LOW;

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

      Serial.println("Waiting for serial data");

      while(!lineAvailable(MAX_LINE, serialInput)) {
        delay(10);

        if(buttonPressed(0)) {
          pumpNumber = 0;
          targetWeight = 5.0;
          liquorName = "Test 5g";
          currentState = tare;
          break;
        }

        if(buttonPressed(1)) {
          pumpNumber = 1;
          targetWeight = 15.0;
          liquorName = "Test 15g (1/2oz)";
          currentState = tare;
          break;
        }
      }

      // In debug mode, a button press will set currentState = tare
      if(currentState != tare) {
        Serial.println(serialInput);
        if(!parseSerialCommand((String)serialInput)) {
          Serial.println("Invalid serial command");
          Serial.println("Format: {pump number} {weight in milliigrams} {name of liquor}");
          Serial.println("eg. 01 00080 3/4oz Bourbon");
          return;
        }
        Serial.print("Pump: ");
        Serial.print(pumpNumber);
        Serial.print(" Weight: ");
        Serial.print(targetWeight);
        Serial.print(" Name: ");
        Serial.println(liquorName);
      
        currentState = tare;
      }
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
        currentWeight = scale.get_units(5);
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

      sprintf(oledWeightString, "%01d.%02dg     ", (int)(currentWeight), abs((int)(currentWeight*100)%100));
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


int buttonStates[2] = {LOW, LOW};
int lastButtonStates[2] = {LOW, LOW};


bool buttonPressed(int num) {
  buttonStates[num] = digitalRead(buttonPins[num]);
  
  if(buttonStates[num] != lastButtonStates[num]) {
    Serial.print("Button ");
    Serial.print(num);
    Serial.print(" is now ");
    Serial.println(buttonStates[num]);
    lastButtonStates[num] = buttonStates[num];
    if(buttonStates[num] == HIGH) {
      return true;
    } else {
      return false;
    }
  } else {
    return false;
  }
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
  targetWeight = ((double)inputString.substring(3,8).toInt()) / 1000.0;
  liquorName = inputString.substring(9);

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

