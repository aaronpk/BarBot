
/* built-in SRAM is 32K mostly free, but a couple of libraries benefit from it as follows
 
   Oled Library uses 0x7c00 - 0x7fff (1K)
   WIFI Library uses 0x7400 - 0x7bff (2K)

   The addresses can be changed in their respective headers of course, but this is how
   the mapping is set by default
*/

#ifndef _PICCOLINO_RAM_H
#define _PICCOLINO_RAM_H

#define CMD_WREN 0x06 //0000 0110 Set Write Enable Latch
#define CMD_WRDI 0x04 //0000 0100 Write Disable
#define CMD_RDSR 0x05 //0000 0101 Read Status Register
#define CMD_WRSR 0x01 //0000 0001 Write Status Register
#define CMD_READ 0x03 //0000 0011 Read Memory Data
#define CMD_WRITE 0x02 //0000 0010 Write Memory Data
 
#define BYTE_MODE (0x00)
#define PAGE_MODE (0x80)
#define STREAM_MODE (0x40)

#define RAM_CS 8 //chip select

class Piccolino_RAM {
public:

  Piccolino_RAM();
  ~Piccolino_RAM();

  void begin(int addr=0);
  int write(int addr, byte *buf, int count=1);
  int read(int addr, byte *buf, int count=1);

private:
	int _ram_start_addr;
};

#endif