#include <Wire.h>
#include <SPI.h>
#include <Piccolino_RAM.h>
#include <Piccolino_OLED_SRAM.h>
#include <HX711.h>
#include <PCF8575.h>

// Which pins are the scale plugged in to
#define SCALE_DATA 3
#define SCALE_CLK 4

// Pins 0 and 1 are SCL and SCA, you probably don't need to change
#define EXPPIN0 0
#define EXPPIN1 1

// Serial communication rate
#define BAUD 57600

// Hardware debug buttons
int buttonPins[2] = {14, 15};

// How many samples of the scale to read per loop?
// A larger number averages out errors better, but may cause 
// overpours since we don't get a chance to stop the pump as often.
#define SAMPLES_PER_LOOP 4

// Between samples, if the weight difference is greater than this
// value (in grams), the sample is tossed out. This smoothes out
// jolts in the measurement such as when the glass is bumped, or
// also if there is noise in the scale.
#define NOISE_THRESHOLD 5

/*
 * Calibration:
 * 
 * Send a serial command of "calibrate"
 * Place a known weight on the scale, and observe the reading.
 * Divide the number shown by the known weight.
 * Enter that number in the calibration parameter below
 */
// #define SCALE_CALIBRATION -1916
#define SCALE_CALIBRATION -1220


/////////////////////////////////////////////////////////////////

HX711 scale(SCALE_DATA, SCALE_CLK);
PCF8575 expander;
Piccolino_OLED_SRAM display;
const byte expanderAddress = 0x20;
//const byte s7sAddress = 0x71;

//bool ledEnabled = false;

void setup() {
  Serial.begin(BAUD);
  Serial.println("*-*-*-*-*-*-*-*-*-*-*-*-*");
  Serial.println("BarBot");
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

  /*
  if(ledEnabled) {
    Serial.println("Setting up LED display");
    clearDisplayI2C();
    setBrightnessI2C(0x255);
    setDecimalsI2C(0b00001111);
    delay(250);
    setDecimalsI2C(0b00000000);
    lcdBarBotAnimation();
  }
  */
  
  expander.begin(expanderAddress);
  expander.pinMode(EXPPIN0, OUTPUT);
  expander.pinMode(EXPPIN1, OUTPUT);

  for(int i=0; i<16; i++) {
    expander.digitalWrite(i, LOW);
  }
  
  scale.set_scale(SCALE_CALIBRATION);

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
  dispensing,
  calibrate,
  test
};

#define MAX_LINE 50

state currentState = waiting;
char serialInput[MAX_LINE+1];
double currentWeight;
double lastWeight = 0;
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

  /*
  if(ledEnabled) {
    // Read the scale
    currentWeight = scale.get_units(SAMPLES_PER_LOOP);
  
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
  */
  
  switch(currentState) {
    case waiting:

      Serial.println("{\"mode\":\"ready\"}");

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
        int command = parseSerialCommand((String)serialInput);
        if(command == 1) {
          /*
          Serial.print("Pump: ");
          Serial.print(pumpNumber);
          Serial.print(" Weight: ");
          Serial.print(targetWeight);
          Serial.print(" Name: ");
          Serial.println(liquorName);
          */
        
          currentState = tare;
        } else if(command == -1) {
          // Calibrate
          Serial.println("Beginning Calibration Mode");
          
          currentState = calibrate;
          display.clear();
          display.setTextColor(WHITE);
          display.setTextSize(1);
          display.setCursor(0,0);
          display.print("Calibration Mode");
          display.update();
          scale.set_scale();
          scale.tare();

        } else if(command == -2) {
          Serial.println("Beginning Test Mode");
          
          currentState = calibrate;
          display.clear();
          display.setTextColor(WHITE);
          display.setTextSize(1);
          display.setCursor(0,0);
          display.print("Test Mode");
          display.update();
          scale.set_scale(SCALE_CALIBRATION);
          scale.tare();

        } else {
          Serial.println("Invalid serial command");
          //Serial.println("Format: {pump number} {weight in milliigrams} {name of liquor}");
          //Serial.println("eg. 01 00080 3/4oz Bourbon");
          Serial.println("{\"mode\":\"error\"}");
          return;
        }
      }
      break;
      
    case tare:
      /*
      if(ledEnabled) {
        setDecimalsI2C(0b00000000);
        s7sSendStringI2C("----");
      }
      */
      
      Serial.println("{\"mode\":\"tare\"}");

      display.clear();
      display.setTextColor(WHITE);
      display.setTextSize(1);
      display.setCursor(0,0);
      display.print(liquorName);
      display.setCursor(0,9);
      sprintf(ledString, "Pump %d", pumpNumber);
      display.print(ledString);
      display.update();

      scale.set_scale(SCALE_CALIBRATION);
      scale.tare();

      Serial.print("{\"mode\":\"starting\",\"pump\":");
      Serial.print(pumpNumber);
      Serial.print(",\"target_weight\":");
      Serial.print(targetWeight, 3);
      Serial.println("}");

      currentState = dispensing;
      
      // Turn on the pump
      expander.digitalWrite(pumpNumber-1, HIGH);
      break;

    case test:
    case calibrate:
      currentWeight = scale.get_units(SAMPLES_PER_LOOP);
      Serial.print("Current weight: ");
      Serial.println(currentWeight);

      display.setTextSize(2);
      sprintf(oledWeightString, "%01d.%01dg   ", (int)(currentWeight), abs((int)(currentWeight*10)%10));
      // display.setCursor(0,52); // for size 1
      display.setCursor(0,42); // for size 2
      display.print(oledWeightString);

      display.update();  

      break;
      
    case dispensing:
      
      //if(!ledEnabled) {
        // Read the scale
        // If the weight LED is enabled, it will have already been read this loop.
        currentWeight = scale.get_units(SAMPLES_PER_LOOP);
      //}

      // Skip this reading if it returned < 0
      if(currentWeight < 0) {
        break;
      }

      // Check that the current value does not deviate by a large amount
      // which helps prevent noise from the scale.
      if(abs(currentWeight - lastWeight) > NOISE_THRESHOLD) {
        Serial.print("{\"mode\":\"skipped\",\"difference\":");
        Serial.print(currentWeight - lastWeight);
        Serial.println("}");
        lastWeight = currentWeight;
        break;
      }
      lastWeight = currentWeight;

      Serial.print("{\"mode\":\"dispensing\",\"pump\":");
      Serial.print(pumpNumber);
      Serial.print(",\"target_weight\":");
      Serial.print(targetWeight, 3);
      Serial.print(",\"current_weight\":");
      Serial.print(currentWeight, 3);
      Serial.println("}");

      percentWeight = (int)(currentWeight / targetWeight * 100);
      if(percentWeight < 0) { 
        percentWeight = 0;
      }

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

      display.setTextSize(2);
      sprintf(oledWeightString, "%01d.%01dg   ", (int)(currentWeight), abs((int)(currentWeight*10)%10));
      // display.setCursor(0,52); // for size 1
      display.setCursor(0,42); // for size 2
      display.print(oledWeightString);

      display.update();  
      
      // If the scale registers greater than the target weight,
      // stop the pump, and reset the state.
      if(currentWeight >= targetWeight) {
        expander.digitalWrite(pumpNumber-1, LOW);
        currentState = waiting;

        /*
        if(ledEnabled) {
          setDecimalsI2C(0b00000000);
          s7sSendStringI2C("DONE");
        }
        */

        Serial.print("{\"mode\":\"complete\",\"final_weight\":");
        Serial.print(currentWeight, 3);
        Serial.print(",\"target_weight\":");
        Serial.print(targetWeight, 3);
        Serial.print(",\"pump\":");
        Serial.print(pumpNumber);
        Serial.println("}");

        display.setCursor(0,9);
        display.setTextSize(1);
        display.print("Done!         ");
        display.update();

      }
      break;

    default:
      break;
  }

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

/*
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
*/

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

int parseSerialCommand(String inputString)
{
  if(inputString.substring(0,9) == "calibrate") {
    return -1;
  }

  if(inputString.substring(0,4) == "test") {
    return -2;
  }

  if(inputString.length() < 8) {
    return 0;
  }

  pumpNumber = (int)inputString.substring(0,2).toInt();
  targetWeight = ((double)inputString.substring(3,8).toInt()) / 1000.0;
  liquorName = inputString.substring(9);

  if(pumpNumber == 0 || targetWeight == 0) {
    return 0;
  }

  return 1;
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

/*
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
*/

