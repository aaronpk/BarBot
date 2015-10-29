#include <SPI.h>
#include <Piccolino_RAM.h>

Piccolino_RAM ram;

void setup()   {                
  Serial.begin(9600);
  ram.begin(); // offset start at 0
}

void loop()                    
{
  byte data_to_chip[17] = "Testing 90123456";
  byte data_from_chip[17] = "                ";
  int i = 0;

// write to memory chip
  ram.write(0,data_to_chip,16);  
  
 // read it back ...
  ram.read(0,data_from_chip,16);
 
  Serial.println((char *)data_from_chip);
  
  delay(1000);                
}
