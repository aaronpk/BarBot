#include <Wire.h>
#include <SPI.h>
#include <Piccolino_RAM.h>
#include <Piccolino_OLED_SRAM.h>
#include <HX711.h>
#include <PCF8575.h>

#define SCALE_DATA 3
#define SCALE_CLK 4

#define SCALE_CALIBRATION 0

HX711 scale(SCALE_DATA, SCALE_CLK);
// PCF8575 expander;
Piccolino_OLED_SRAM display;

/*
 How to Calibrate Your Scale

 1. Call set_scale() with no parameter.
 2. Call tare() with no parameter.
 3. Place a known weight on the scale and call get_units(10).
 4. Divide the result in step 3 to your known weight. You should get about the parameter you need to pass to set_scale.
 5. Adjust the parameter in step 4 until you get an accurate reading.
*/


const byte expanderAddress = 0x20;

const int button1pin = 14;
const int button2pin = 15;

void setup() {
  Serial.begin(57600);

  Serial.println("*-*-*-*-*-*-*-*-*-*-*-*-*");
  Serial.println("*-*-*-*-*-*-*-*-*-*-*-*-*");
  Serial.println("BarBot Calibration");
  Serial.println("*-*-*-*-*-*-*-*-*-*-*-*-*");
  Serial.println("*-*-*-*-*-*-*-*-*-*-*-*-*");

  Wire.begin();

  // expander.begin(expanderAddress);
  // expander.pinMode(0, OUTPUT);
  // expander.pinMode(1, OUTPUT);

  display.begin();
  display.clear();

  display.setTextSize(2);
  display.setTextColor(WHITE);

  pinMode(button1pin, INPUT);
  pinMode(button2pin, INPUT);


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
  
  Serial.println("Resetting scale...");
  scale.set_scale(SCALE_CALIBRATION);

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
    
  Serial.println("Tare...");
  scale.tare();
  Serial.println("Ready");
}

double currentWeight;
float currentValue;
char oledValueString[10];
char oledWeightString[10];

int lastButton1 = LOW;
int lastButton2 = LOW;

void loop() {
  /*
  int button1 = digitalRead(button1pin);
  int button2 = digitalRead(button2pin);

  if(button1 != lastButton1) {
    Serial.println("Button 1 changed");
    lastButton1 = button1;
  }
  if(button2 != lastButton2) {
    Serial.println("Button 2 changed");
    lastButton2 = button2;
    if(button2 == HIGH) {
      scale.tare();
    }
  }

  expander.digitalWrite(0, button1);
  */
  
  currentWeight = scale.get_units(2);
  Serial.println(currentWeight);

  sprintf(oledWeightString, "%01d.%03dg     ", (int)(currentWeight), abs((int)(currentWeight*1000)%1000));

  display.setCursor(0, 0);
  display.print(oledWeightString);

  //display.setCursor(0,52);
  //display.print(oledWeightString);
  display.update();
}
