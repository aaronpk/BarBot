//#include <avr/pgmspace.h>


/*************************************************************
* VERY (very) basic I2C library for the SSD1306 OLED LCD     *
* Some of the functions were borrowed from Adafruit's GFX    *
* Library, so thank her for them (see below).                *
* This version of the OLED library uses 1K of SRAM freeing   *
* almost 1K of local RAM - it's slower but good enough for   *
* most occasions!                                            *
*                                                            *
* Version: 1.2 - June 2015                                   *
* Version: 1.3 - July 2015                                   *
*  - Added drawRect (draw a rectangle)                       *
*  - Added GRAY color flag (draw lines every second pixel)   *
*                                                            *
* Written by fr333n3rgy@yahoo.com for WCTEK.com              *
*************************************************************/


#include "Piccolino_OLED_SRAM.h"
#include "glcdfont.c"

Piccolino_RAM  _v_ram;

Piccolino_OLED_SRAM::Piccolino_OLED_SRAM() {
	// nothing here now- all done in 'begin'
}


void Piccolino_OLED_SRAM::begin(uint8_t vccstate, uint8_t i2caddr) {
    _i2caddr = i2caddr;
    cursor_x=0;
    cursor_y=0;
    textcolor=WHITE;
    textbgcolor=BLACK;

    _v_ram.begin(ADDR_VIDEOBUFFER);

    // set pin directions
    // I2C Init
    Wire.begin(); // Is this the right place for this?
  
    // Init sequence for 128x64 OLED module
    ssd1306_command(SSD1306_DISPLAYOFF);                    // 0xAE
    ssd1306_command(SSD1306_SETDISPLAYCLOCKDIV);            // 0xD5
    ssd1306_command(0x80);                                  // the suggested ratio 0x80
    ssd1306_command(SSD1306_SETMULTIPLEX);                  // 0xA8
    ssd1306_command(0x3F);
    ssd1306_command(SSD1306_SETDISPLAYOFFSET);              // 0xD3
    ssd1306_command(0x0);                                   // offset
    ssd1306_command(SSD1306_SETSTARTLINE | 0x0);            // line #0
    ssd1306_command(SSD1306_CHARGEPUMP);                    // 0x8D
    if (vccstate == SSD1306_EXTERNALVCC)
      { ssd1306_command(0x10); }
    else
      { ssd1306_command(0x14); }
    ssd1306_command(SSD1306_MEMORYMODE);                    // 0x20
    ssd1306_command(0x00);                                  // 0x0 act like ks0108
    ssd1306_command(SSD1306_SEGREMAP | 0x1);
    ssd1306_command(SSD1306_COMSCANDEC);
    ssd1306_command(SSD1306_SETCOMPINS);                    // 0xDA
    ssd1306_command(0x12);
    ssd1306_command(SSD1306_SETCONTRAST);                   // 0x81
    if (vccstate == SSD1306_EXTERNALVCC)
      { ssd1306_command(0x9F); }
    else
      { ssd1306_command(0xCF); }
    ssd1306_command(SSD1306_SETPRECHARGE);                  // 0xd9
    if (vccstate == SSD1306_EXTERNALVCC)
      { ssd1306_command(0x22); }
    else
      { ssd1306_command(0xF1); }
    ssd1306_command(SSD1306_SETVCOMDETECT);                 // 0xDB
    ssd1306_command(0x40);
    ssd1306_command(SSD1306_DISPLAYALLON_RESUME);           // 0xA4
    ssd1306_command(SSD1306_NORMALDISPLAY);                 // 0xA6

    clear(); // clear sram ...
    setTextColor(WHITE);
    setTextSize(1);
    update();

    ssd1306_command(SSD1306_DISPLAYON);//--turn on oled panel

}

void Piccolino_OLED_SRAM::clearpart(int from)
{
clearpart(from,7);
}

void Piccolino_OLED_SRAM::clearpart(int from, int tto)
{
  memset(buff,0,128);//clear the back buffer.
  for(int f=from; f<=tto; f++)
    _v_ram.write(f*SSD1306_LCDWIDTH, buff, 128);
}

void Piccolino_OLED_SRAM::invertDisplay(uint8_t i) {
  if (i) {
    ssd1306_command(SSD1306_INVERTDISPLAY);
  } else {
    ssd1306_command(SSD1306_NORMALDISPLAY);
  }
}

void Piccolino_OLED_SRAM::ssd1306_command(uint8_t c) {
    // I2C
    uint8_t control = 0x00;   // Co = 0, D/C = 0
    Wire.beginTransmission(_i2caddr);
    Wire.write(control);
    Wire.write(c);
    Wire.endTransmission();
}


void Piccolino_OLED_SRAM::ssd1306_data(uint8_t c) {
    // I2C
    uint8_t control = 0x40;   // Co = 0, D/C = 1
    Wire.beginTransmission(_i2caddr);
    Wire.write(control);
    Wire.write(c);
    Wire.endTransmission();
}

void Piccolino_OLED_SRAM::update()
{
  unsigned char x;
  int pos=0;

    ssd1306_command(SSD1306_SETLOWCOLUMN | 0x0);  // low col = 0
    ssd1306_command(SSD1306_SETHIGHCOLUMN | 0x0);  // hi col = 0
    ssd1306_command(SSD1306_SETSTARTLINE | 0x0); // line #0

    // save I2C bitrate
    uint8_t twbrbackup = TWBR;
    TWBR = 18; // upgrade to 400KHz!

for (char rowID=0; rowID<8; rowID++){

  pos=0;
      
    Wire.beginTransmission(_i2caddr);
    Wire.write(0x00); // send command
    Wire.write(0xB0+rowID);
    Wire.write(0);
    Wire.endTransmission();

    _v_ram.read(rowID*SSD1306_LCDWIDTH, buff, 128);

        for(byte j = 0; j < 8; j++){
            Wire.beginTransmission(_i2caddr);
            Wire.write(0x40);
            for (byte k = 0; k < 16; k++) {
                Wire.write(buff[pos++]);
            }
        Wire.endTransmission();
        }

}

    TWBR = twbrbackup;   
}

void Piccolino_OLED_SRAM::clearLine(byte line)
{
    ssd1306_command(SSD1306_SETLOWCOLUMN | 0x0);  // low col = 0
    ssd1306_command(SSD1306_SETHIGHCOLUMN | 0x0);  // hi col = 0
    ssd1306_command(SSD1306_SETSTARTLINE | 0x0); // line #0

    // save I2C bitrate
    uint8_t twbrbackup = TWBR;
    TWBR = 18; // upgrade to 400KHz!

    // send a bunch of data in one xmission
    ssd1306_command(0xB0 + line);//set page address
    ssd1306_command(0);//set lower column address
    ssd1306_command(0x10);//set higher column address

    for(byte j = 0; j < 8; j++){
        Wire.beginTransmission(_i2caddr);
        Wire.write(0x40);
        for (byte k = 0; k < 16; k++) {
            Wire.write(0);
        }
        Wire.endTransmission();
    }

    TWBR = twbrbackup;
}


void Piccolino_OLED_SRAM::updateRow(int rowID)
{
  unsigned char x;
  int pos=0;

    ssd1306_command(SSD1306_SETLOWCOLUMN | 0x0);  // low col = 0
    ssd1306_command(SSD1306_SETHIGHCOLUMN | 0x0);  // hi col = 0
    ssd1306_command(SSD1306_SETSTARTLINE | 0x0); // line #0

    // save I2C bitrate
    uint8_t twbrbackup = TWBR;
    TWBR = 18; // upgrade to 400KHz!

    
    Wire.beginTransmission(_i2caddr);
    Wire.write(0x00); // send command
    Wire.write(0xB0+rowID);
    Wire.write(0);
    Wire.endTransmission();

   _v_ram.read(rowID*SSD1306_LCDWIDTH, buff, 128);

        for(byte j = 0; j < 8; j++){
            Wire.beginTransmission(_i2caddr);
            Wire.write(0x40);
            for (byte k = 0; k < 16; k++) {
                Wire.write(buff[pos++]);
            }
		    Wire.endTransmission();
        }
    

    TWBR = twbrbackup;   
}

void Piccolino_OLED_SRAM::updateRow(int startID, int endID)
{
  unsigned char y =0;
  for(y=startID; y<endID; y++)
  {
    updateRow(y);
  }
}

// Original concept borrowed from Adafruit's GFX library modified for Piccolino by AS
void Piccolino_OLED_SRAM::drawPixel(int16_t x, int16_t y, uint16_t color)
{
  int row;
  unsigned char offset;
  unsigned char  preData; //previous data.
  unsigned char val;

if (x>127||y>63) return;
if (x<0||y<0) return;

  row = y/8;
  offset =y%8;

  _v_ram.read((row*SSD1306_LCDWIDTH)+x, buff);
  preData = buff[0];

  //set pixel;
  val = 1<<offset;

  if(color!=0)
  {//white! set bit.
    buff[0] = preData | val;
  }
  else
  {       //black! clear bit.
    buff[0] = preData & (~val);
  }

  _v_ram.write((row*SSD1306_LCDWIDTH)+x, buff);  

}

void Piccolino_OLED_SRAM::drawRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color) {  
  drawLine(x, y, x, y+h, color);
  drawLine(x+w, y, x+w, y+h, color);
  drawLine(x, y, x+w, y, color);
  drawLine(x, y+h, x+w, y+h, color);
}

void Piccolino_OLED_SRAM::clear()
{
  	clearpart(0);
}

// Original concept borrowed from Adafruit's GFX library
void Piccolino_OLED_SRAM::drawChar(int16_t x, int16_t y, unsigned char c, uint16_t color, uint16_t bg, uint8_t size) {

  for (int8_t i=0; i<6; i++ ) {
    uint8_t line;
    if (i == 5) 
      line = 0x0;
    else 
      line = pgm_read_byte(font+(c*5)+i);

    for (int8_t j = 0; j<8; j++) {
      if (line & 0x1) {
        if (size == 1) // default size
          drawPixel(x+i, y+j, color);
        else {  // big size
          fillRect(x+(i*size), y+(j*size), size, size, color);
        } 
      } else if (bg != color) {
        if (size == 1) // default size
          drawPixel(x+i, y+j, bg);
        else {  // big size
          fillRect(x+i*size, y+j*size, size, size, bg);
        }
      }
      line >>= 1;
    }
  }
}

void Piccolino_OLED_SRAM::setCursor(int16_t x, int16_t y) {
  cursor_x = x;
  cursor_y = y;
}

void Piccolino_OLED_SRAM::setTextSize(uint8_t s) {
  textsize = (s > 0) ? s : 1;
}

void Piccolino_OLED_SRAM::setTextColor(uint16_t c) {
  textcolor = c;
}
void Piccolino_OLED_SRAM::setTextColor(uint16_t c,uint16_t b) {
  textcolor = c;
  textbgcolor = b;
}

void Piccolino_OLED_SRAM::fillRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color) {
 for (int16_t i=x; i<x+w; i++) {
    drawLine(i, y, i, y+h-1, color);
  }
}

// Bresenham's algorithm - borrowed from Adafruit's GFX Library
void Piccolino_OLED_SRAM::drawLine(int16_t x0, int16_t y0, int16_t x1, int16_t y1, uint16_t color) {
  int16_t steep = abs(y1 - y0) > abs(x1 - x0);

  if (steep) {
    swap(x0, y0);
    swap(x1, y1);
  }

  if (x0 > x1) {
    swap(x0, x1);
    swap(y0, y1);
  }

  int16_t dx, dy;
  dx = x1 - x0;
  dy = abs(y1 - y0);

  int16_t err = dx / 2;
  int16_t ystep;

  if (y0 < y1) {
    ystep = 1;
  } else {
    ystep = -1;
  }

   for (; x0<=x1; x0++) {
    if (steep) {
      if(color==GRAY)
        drawPixel(y0, x0, !(x0%2));
      else
        drawPixel(y0, x0, color);
    } else {
      if(color==GRAY)
        drawPixel(x0, y0, !(x0%2));
      else
        drawPixel(x0, y0, color);
    }
    err -= dy;
    if (err < 0) {
      y0 += ystep;
      err += dx;
    }
  }
}


size_t Piccolino_OLED_SRAM::write(uint8_t c) {
  if (c == '\n') {
    cursor_y += textsize*8;
    cursor_x  = 0;
  } else if (c == '\r') {
    // skip em
  } else {
    drawChar(1+cursor_x, 1+cursor_y, c, textcolor, textbgcolor, textsize);
    cursor_x += textsize*6;
    if (wrap && (cursor_x > (SSD1306_LCDWIDTH - textsize*6))) {
      cursor_y += textsize*8;
      cursor_x = 0;
    }
  }
  return 1;
}

Piccolino_OLED_SRAM::~Piccolino_OLED_SRAM()
{

}
