//#include <avr/pgmspace.h>

/*************************************************************
* This library supports FRAM and SRAM devices on the new     *
* WCT Pro Mini+ and Piccolino devices                        *
*                                                            *
* To use FRAM, add a #define FRAM entry before including the *
* header in your sketch                                      *
*                                                            *
* Written by Alex Sardo - WCTEK.com                          *
*************************************************************/
#include <SPI.h>

#include "Piccolino_RAM.h"

Piccolino_RAM::Piccolino_RAM() {
	// nothing here now -- all done in 'begin'
}


void Piccolino_RAM::begin(int addr) {
  pinMode(RAM_CS, OUTPUT);
  digitalWrite(RAM_CS, HIGH);
  
  _ram_start_addr=addr;

  //Setting up the SPI bus
  SPI.begin();
  SPI.setDataMode(SPI_MODE0);  
  SPI.setBitOrder(MSBFIRST);
  SPI.setClockDivider(SPI_CLOCK_DIV2);

#ifndef FRAM
  digitalWrite(RAM_CS, LOW);
    SPI.transfer(CMD_WRSR);
    SPI.transfer(0x40); // stream mode implicit
  digitalWrite(RAM_CS, HIGH);
#endif

}

int Piccolino_RAM::write(int addr, byte *buf, int count)
{

  addr+=_ram_start_addr;

#ifdef RAM
  digitalWrite(RAM_CS, LOW);   
  SPI.transfer(CMD_WREN);  //write enable 
  digitalWrite(RAM_CS, HIGH);
#endif
   
  digitalWrite(RAM_CS, LOW);
  SPI.transfer(CMD_WRITE); //write command
  SPI.transfer((char)(addr >> 8));
  SPI.transfer((char)addr);
   
  for (int i = 0;i < count;i++) 
    SPI.transfer(buf[i]);
 
  digitalWrite(RAM_CS, HIGH);
   
  return 0;
}
 
int Piccolino_RAM::read(int addr, byte *buf, int count)
{
  
  addr+=_ram_start_addr;

  digitalWrite(RAM_CS, LOW);
   
  SPI.transfer(CMD_READ);
  SPI.transfer((char)(addr >> 8));
  SPI.transfer((char)addr);
   
  for (int i=0; i < count; i++) 
    buf[i] = SPI.transfer(0xff); // anything will trigger a byte to be sent back ...
 
  digitalWrite(RAM_CS, HIGH);
   
  return 0;
}

Piccolino_RAM::~Piccolino_RAM() {
  // nothing here now -- all done in 'begin'
}


