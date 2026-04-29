// toled.h - Tiny OLED driver (only 128x32, only I2C, only for ESP32)
#include <Arduino.h> // Serial.printf
#include <Wire.h>    // Wire.write
#include "toled.h"   // own


#define TOLED_SIZE     (TOLED_WIDTH*TOLED_HEIGHT/8)
#define I2CADDR        0x3C


static uint8_t toled_framebuf[TOLED_SIZE];
// Frame buffer layout of the OLED with 128 columns and 32 rows
// Every rectangle in the diagram below is one byte. It holds 8 pixels,
// which is a quarter column of the OLED in landscape.
//      0     1     2        127
//    +---+ +---+ +---+     +---+
//  0 |LSB| |   | |   |     |   |
//  : | 0 | | 1 | | 2 | ... |127|
//  7 |MSB| |   | |   |     |   |
//    +---+ +---+ +---+     +---+
//    +---+ +---+ +---+     +---+
//  8 |   | |   | |   |     |   |
//  : |128| |   | |   | ... |255|
// 15 |   | |   | |   |     |   |
//    +---+ +---+ +---+     +---+
//    +---+ +---+ +---+     +---+
// 16 |   | |   | |   |     |   |
//  : |256| |   | |   | ... |383|
// 23 |   | |   | |   |     |   |
//    +---+ +---+ +---+     +---+
//    +---+ +---+ +---+     +---+
// 24 |   | |   | |   |     |   |
//  : |384| |   | |   | ... |511|
// 32 |   | |   | |   |     |   |
//    +---+ +---+ +---+     +---+


// Applies mask msk to toled_framebuf[loc] using color col
static inline void toled_framebuf_mask(int loc, int msk, int col) {
  if( loc<0 || loc>=TOLED_SIZE ) return;
  switch( col ) {
    case TOLED_COL_WHITE: toled_framebuf[loc] |=  msk; break;
    case TOLED_COL_BLACK: toled_framebuf[loc] &= ~msk; break;
    case TOLED_COL_FLIP : toled_framebuf[loc] ^=  msk; break;
  }
}


// === I2C REGISTERS ======================================


// Commands of the SSD1306 OLED
// See https://iotexpert.com/debugging-ssd1306-display-problems/

// See datasheet SSD1306 128 x 64 Dot Matrix OLED/PLED Segment/Common Driver with Controller
// https://www.digikey.com/htmldatasheets/production/2047793/0/0/1/ssd1306.html

////////////////////////////////////////////////////////////////////////
// Page 28, Chap 9, Table 9-1, section 1.Fundamental
////////////////////////////////////////////////////////////////////////
// Command with 1 arg to select 1 out of 256 contrast steps.
#define TOLED_SETCONTRAST                              0x81
// +arg1: 0-0xFF Contrast value (reset = 0x7F)
  #define TOLED_SETCONTRAST_HALF                       0x7F
  #define TOLED_SETCONTRAST_MAX                        0xFF

// Command with embedded arg to resume/ignore displaying data
#define TOLED_DISPLAYALLONRESUME                       0xA4 // (reset)
#define TOLED_DISPLAYALLONIGNORE                       0xA5

// Command with embedded arg to select normal or inverse display
#define TOLED_DISPLAYNORMAL                            0xA6
#define TOLED_DISPLAYINVERT                            0xA7

// Command with embedded arg to turn screen off/on
#define TOLED_DISPLAYOFF                               0xAE
#define TOLED_DISPLAYON                                0xAF

////////////////////////////////////////////////////////////////////////
// Page 28, Chap 9, Table 9-1, section 2.Scrolling
////////////////////////////////////////////////////////////////////////
// Command to deavtive scrolling
#define TOLED_DEACTIVATE_SCROLL                        0x2E

////////////////////////////////////////////////////////////////////////
// Page 30, Chap 9, Table 9-1, section Address Setting
////////////////////////////////////////////////////////////////////////
// Command with embedded arg to set the lower nibble of the column start address register for Page Addressing Mode
#define TOLED_SETLOWNIBBLEOFPAGE(addr)                 (0x00 | ((addr)&0x0F))

// Command with embedded arg to set the higher nibble of the column start address register for Page Addressing Mode
#define TOLED_SETHIGHNIBBLEOFPAGE(addr)                (0x01 | ((addr)&0x0F))

// Command with 1 arg to set memory addressing mode
#define TOLED_SETMEMORYMODE                            0x20
// +arg1: legal arg values are:
  #define TOLED_SETMEMORYMODE_HOR                      0x00
  #define TOLED_SETMEMORYMODE_VER                      0x01
  #define TOLED_SETMEMORYMODE_PAGE                     0x02

// Command with 2 args to set column address
#define TOLED_SETCOLUMNADDR                            0x21
// +arg1: 0-127 column start address
// +arg2:0-127 column end address

// Command with 2 args to set page address
#define TOLED_SETPAGEADDR                              0x22
// +arg1: 0-7 page start address
// +arg2: 0-7 page end Address

// Command with embedded arg to set page start address
#define TOLED_SETPAGESTART(page)                       (0xB0 | ((page)&0x07))

////////////////////////////////////////////////////////////////////////
// Page 31, Chap 9, Table 9-1, section Hardware Configuration
////////////////////////////////////////////////////////////////////////
// Command with embedded arg to set address startline from 0-63 (6-bits)
#define TOLED_SETSTARTLINE(line)                       (0x40 | ((line)&0x3F))
#define TOLED_SETSTARTLINE_ZERO                        0x40

// Command with embedded arg to remap segment
#define TOLED_SEGREMAPNORMAL                           0xA0
#define TOLED_SEGREMAPINV                              0xA1

// Command with 1 arg to set multiplex ratio
#define TOLED_SETMULTIPLEX                             0xA8
// +arg1: number of rows-1

// Command with embedded arg to set com output scan direction
#define TOLED_COMSCANINC                               0xC0
#define TOLED_COMSCANDEC                               0xC8

// Command with 1 arg to set vertical shift
#define TOLED_SETDISPLAYOFFSET                         0xD3
// +arg1: number of rows to shift (0..63)

// Command with 1 arg to to set hardware com pins configuration
#define TOLED_SETCOMPINS                               0xDA
// +arg1: legal arg values are:
  #define TOLED_SETCOMPINS_DIS_SEQ                     0x02
  #define TOLED_SETCOMPINS_DIS_ALT                     0x12
  #define TOLED_SETCOMPINS_ENA_SEQ                     0x22
  #define TOLED_SETCOMPINS_ENA_ALT                     0x32

////////////////////////////////////////////////////////////////////////
// Page 32, Chap 9, Table 9-1, section Timing and Driving Scheme Settings
////////////////////////////////////////////////////////////////////////
// Command with 1 arg to set set display clock divide ratio/oscillator frequency
#define TOLED_SETDISPLAYCLOCKDIV                       0xD5
// +arg1: lower nibble divide ratio, upper nibble osc freq (0x80 is reset)

// Command with 1 arg to set set Set Pre-charge Period
#define TOLED_SETPRECHARGE                             0xD9
// +arg1: lower nibble phase 1 of clock, upper nibble phase 2 of clock (0x22 is reset)

// Command with 1 arg to set set Set Pre-charge Period
#define TOLED_SETVCOMDESELECT                          0xDB
// +arg1: legal arg values 0x00, 0x20, 0x30
  #define TOLED_SETVCOMDESELECT_065                    0x00
  #define TOLED_SETVCOMDESELECT_077                    0x20
  #define TOLED_SETVCOMDESELECT_083                    0x30

// Command with no effect
#define TOLED_NOP                                      0xE3

////////////////////////////////////////////////////////////////////////
// Page 3, Chap 2 Charge Pump
////////////////////////////////////////////////////////////////////////
// Command with 1 arg to set set charge pump setting
#define TOLED_CHARGEPUMP                               0x8D
// +arg1: legal arg values are:
  #define TOLED_CHARGEPUMP_OFF                         0x10
  #define TOLED_CHARGEPUMP_ON                          0x14


static const uint8_t oled_config_cmds[] = {
  // Fundamental Commands
  TOLED_DISPLAYOFF,            // 0xAE Screen Off
  TOLED_SETCONTRAST,           // 0x81 Set contrast
    TOLED_SETCONTRAST_HALF,
  TOLED_DISPLAYNORMAL,         // 0xA6 Set normal display

  // Scrolling Commands
  TOLED_DEACTIVATE_SCROLL,     // 0x2E Deactive scrolling

  // Addressing Commands
  TOLED_SETMEMORYMODE,         // 0x20 Set memory address mode
    TOLED_SETMEMORYMODE_HOR,   // Horizontal

  // Hardware Configuration Commands
  TOLED_SETSTARTLINE_ZERO,     // 0x40 Set startline
  TOLED_SEGREMAPINV,           // 0xA1 Set segment re-map
  TOLED_SETMULTIPLEX,          // 0xA8 Set multiplex ratio
    0x1F,                      // TOLED_HEIGHT-1 for 128x32
  TOLED_COMSCANDEC,            // 0xC0 Set COM output scan direction
  TOLED_SETDISPLAYOFFSET,      // 0xD3 Set display offset
    0x00,                      //
  TOLED_SETCOMPINS,            // 0xDA Set COM pins hardware configuration
    TOLED_SETCOMPINS_DIS_SEQ,

  // Timing and Driving Settings
  TOLED_SETDISPLAYCLOCKDIV,    // 0xD5 Set display oscillator frequency 0-0xF & clock divide ratio 0-0xF
    0x80,                      // Reset value 0x80
  TOLED_SETPRECHARGE,          // 0xD9 Set pre-charged period
    0xD9,                      // Reset value is 0x22, but this works for TOLED_SETMULTIPLEX 32
  TOLED_SETVCOMDESELECT,       // 0xDB Set VCOMH deselected level
    TOLED_SETVCOMDESELECT_077, // Default

  // Charge pump regulator
  TOLED_CHARGEPUMP,            // 0x8D Set charge pump
    TOLED_CHARGEPUMP_ON,       // 0x14 VCC generated by internal DC/DC circuit

  // Turn the screen back on...
  TOLED_DISPLAYALLONRESUME,    // 0xA4 Set entire display on/off
  TOLED_DISPLAYON,             // 0xAF Set display on
};


// Sends the cmds to the OLED display; returns I2C transaction code (0=ok)
static int toled_cmds(const uint8_t *cmds, int count) {
  Wire.beginTransmission(I2CADDR);
  Wire.write( 0x00 ); // Control byte: Continuation=0, Data/Command#=0
  Wire.write( cmds, count );
  return Wire.endTransmission();
}


// Inits screen (using hardwired I2C address 3C).
// Caller must have setup I2C pins and bus speed.
//   Wire.begin(8,18);
//   Wire.setClock(1000*1000); // Works for OLED, too high for other I2C devices on the bus
void toled_init() {
  int res = toled_cmds( oled_config_cmds, sizeof oled_config_cmds);
  (void)res; // not reliable: if( res!=0 ) Serial.printf("toled: error %d; could not configure OLED on I2C address %02X\n", res, I2CADDR);
  toled_clear();
}


// Clears the frame buffer.
void toled_clear(void) {
  memset(toled_framebuf, 0, TOLED_SIZE );
}


// Commits the frame buffer (send it to the OLED).
void toled_commit(void) {
  // Send coordinates
  static const uint8_t cmds[] = {
    TOLED_SETPAGEADDR  , 0, 0xFF,
    TOLED_SETCOLUMNADDR, 0, TOLED_WIDTH-1
  };
  toled_cmds(cmds, sizeof cmds);
  // Send content
  #define CHUNK_SIZE (min(256, I2C_BUFFER_LENGTH)-1) // 256 is max of OLED, 256 is max of I2C, -1 because one extra byte is transmitted
  int remaining= TOLED_SIZE;
  for(int chunk=0; remaining>0; chunk+=CHUNK_SIZE) {
    Wire.beginTransmission(I2CADDR);
    Wire.write( 0x40 ); // one extra byte transmitted
    int send= min(CHUNK_SIZE,remaining);
    Wire.write( toled_framebuf+chunk, send );
    remaining-= send;
    Wire.endTransmission();
  }
}


// === PIXELS, LINES, RECTS ===============================


// Adds pixel to frame buffer.
// Note x=0..127, y=0..31, col=TOLED_COL_XXX.
void toled_pixel(int x, int y, int col) {
  if( x<0 || x>=TOLED_WIDTH || y<0 || y>=TOLED_HEIGHT ) return;
  int loc= x + TOLED_WIDTH*(y/8); // location in frame buffer
  int msk= 1 << (y & 7);          // mask to apply
  toled_framebuf_mask(loc,msk,col);
}


// Adds vertical line to frame buffer.
// Note x=0..127, y0=0..31, y1=0..31, col=TOLED_COL_XXX.
void toled_verline(int x, int y0, int y1, int col ) {
  if( x <0 || x >=TOLED_WIDTH  ) return;
  if( y0<0 || y0>=TOLED_HEIGHT ) return;
  if( y1<0 || y1>=TOLED_HEIGHT ) return;
  if( y0>y1 ) return;

  int loc0= x + TOLED_WIDTH*(y0/8);
  int loc1= x + TOLED_WIDTH*(y1/8);

  int msk0= 0xFF & ~((1 << (y0 & 7))-1);
  int msk1= (1 << ((y1 & 7)+1))-1;
  // Correct masks for short verline 
  if( loc0==loc1 ) { msk0&=msk1; msk1&=msk0; }

  if(  msk0 != 0xFF ) { // pre part of line here
    toled_framebuf_mask(loc0,msk0,col);
    loc0= loc0+TOLED_WIDTH;
  }

  if(  msk1 != 0xFF ) { // post part of line here
    toled_framebuf_mask(loc1,msk1,col);
    loc1= loc1-TOLED_WIDTH; // post part by loop
  }

  for( int loc=loc0; loc<=loc1; loc+=TOLED_WIDTH ) {
    toled_framebuf_mask(loc,0xFF,col);
  }
}


// Adds horizontal line to frame buffer.
// Note x0=0..127, x1=0..127, y=0..31, col=TOLED_COL_XXX.
void toled_horline(int x0, int x1, int y, int col ) {
  if( x0<0 || x0>=TOLED_WIDTH  ) return;
  if( x1<0 || x1>=TOLED_WIDTH  ) return;
  if( x0>x1 ) return;
  if( y <0 || y >=TOLED_HEIGHT ) return;

  int loc0= x0 + TOLED_WIDTH*(y/8);
  int loc1= x1 + TOLED_WIDTH*(y/8);
  int msk= 1 << (y & 7);

  for( int loc=loc0; loc<=loc1; loc++ ) {
    toled_framebuf_mask(loc,msk,col);
  }

}


// Adds filled rectangle to frame buffer.
// Note x0=0..127, y0=0..31, x1=0..127, y1=0..31, col=TOLED_COL_XXX.
void toled_fillrect(int x0, int y0, int x1, int y1, int col ) {
  if( x0<0 || x0>=TOLED_WIDTH  ) return;
  if( y0<0 || y0>=TOLED_HEIGHT ) return;
  if( x0>x1 ) return;
  if( x1<0 || x1>=TOLED_WIDTH  ) return;
  if( y1<0 || y1>=TOLED_HEIGHT ) return;
  if( y0>y1 ) return;

  int msk0= 0xFF & ~((1 << (y0 & 7))-1);
  int msk1= (1 << ((y1 & 7)+1))-1;
  // Correct masks for short rects 
  if( y0/8==y1/8 ) { msk0&=msk1; msk1&=msk0; }

  for( int x=x0; x<=x1; x++ ) {
    int loc0= x + TOLED_WIDTH*(y0/8);
    int loc1= x + TOLED_WIDTH*(y1/8);

    if( msk0 != 0xFF ) { // pre part of line here
      toled_framebuf_mask(loc0,msk0,col);
      loc0= loc0+TOLED_WIDTH;
    }

    if( msk1 != 0xFF ) { // post part of line here
      toled_framebuf_mask(loc1,msk1,col);
      loc1= loc1-TOLED_WIDTH; // post part by loop
    }

    for( int loc=loc0; loc<=loc1; loc+=TOLED_WIDTH ) {
      toled_framebuf_mask(loc,0xFF,col);
    }
  }
}


// Adds open rectangle to frame buffer.
// Note x0=0..127, y0=0..31, x1=0..127, y1=0..31, col=TOLED_COL_XXX.
void toled_openrect(int x0, int y0, int x1, int y1, int col ) {
  toled_verline(x0, y0, y1, col);
  toled_horline(x0+1, x1-1, y0, col); // +/-1 to prevent overlapping corners (TOLED_COL_FLIP)
  toled_horline(x0+1, x1-1, y1, col);
  toled_verline(x1, y0, y1, col);
}


// === TEXT ===============================================


// Cursor for text; set by toled_cursor, moved by toled_char and toled_str.
static int toled_x;
static int toled_y;


// Sets cursor for text drawing (toled_char and toled_str)
void toled_cursor(int x, int y) {
  toled_x = x;
  toled_y = y;
}


// Hardwired access to font data (on separate files)
void toled_mono5 (char ch, int * width, int * height, const uint8_t ** bmp );
void toled_sans5 (char ch, int * width, int * height, const uint8_t ** bmp );
void toled_sans8 (char ch, int * width, int * height, const uint8_t ** bmp );
void toled_sans10(char ch, int * width, int * height, const uint8_t ** bmp );
void toled_sans12(char ch, int * width, int * height, const uint8_t ** bmp );
void toled_sans14(char ch, int * width, int * height, const uint8_t ** bmp );
// Generic type to access font data
typedef void (*toled_fontdata_t)(char ch, int * width, int * height, const uint8_t ** bmp );


// Available fonts in this library
static const toled_fontdata_t toled_fontdatas[] = { 
  toled_mono5, toled_sans5, toled_sans8, toled_sans10, toled_sans12, toled_sans14 
};


// These settings record how text will be rendered
static toled_fontdata_t toled_fontdata  = toled_sans8;
static int              toled_kern      = 1; // space between any two chars
static int              toled_color     = TOLED_COL_WHITE;
static int              toled_margin    = 0; // margin, only applied when wrapping


// Sets font, color, kerning (space between chars in pix), and margin (in pix) for wrap around.
void toled_font(toled_font_t font, int col, int kern, int margin ) {
  toled_fontdata= toled_fontdatas[font];
  toled_kern= kern;
  toled_color= col;
  toled_margin = margin;
}


// Adds character to frame buffer at cursor.
// The cursor is where the upper left pixel of the char will be drawn.
void toled_char(char ch) {
  // Lookup bitmap in font
  int width;
  int height;
  const uint8_t * bmp;
  toled_fontdata(ch,&width,&height,&bmp);
  if( bmp==0 ) return;
  #if 0
    Serial.printf("char '%c' width %d height %d\n", ch, width, height);
    Serial.printf("bmp ");
    for( int ix=0; ix<height*(width/8+1); ix++ )
      Serial.printf(" %02X",bmp[ix]);
    Serial.printf("\n");
  #endif
  // Check if char fits, if not wrap
  if( toled_x + width + toled_margin > TOLED_WIDTH ) {
    toled_x = toled_margin;
    toled_y += height;
    // if( toled_y + height > TOLED_HEIGHT ) toled_y = 0;
  }
  // Draw character, pixel by pixel
  int stride= (width-1)/8+1; // ceil(width/8) is width of the char in bytes
  for( int y=0; y<height; y++ ) {
    for( int x=0; x<width; x++ ) {
      int ix = y*stride + x/8;
      int mask = 1<<(7-x%8);
      int draw = bmp[ix] & mask;
      //Serial.printf("%d", draw!=0 );
      if( draw ) {
        int xx= toled_x+x;
        int yy= toled_y+y;
        int loc= xx + TOLED_WIDTH*(yy/8); 
        int msk= 1 << (yy & 7);
        toled_framebuf_mask(loc,msk,toled_color);
      }
    }
    //Serial.printf("\n");
  }
  toled_x += width + toled_kern;
}


// Adds string to frame buffer at cursor.
// The cursor is where the upper left pixel of the char will be drawn.
void toled_str(const char *s) {
  while( *s ) toled_char(*s++);
}


// Returns the width of a string in pixels with current font settings.
int toled_charwidth(char ch) {
  // Lookup bitmap in font
  int width;
  int height;
  const uint8_t * bmp;
  toled_fontdata(ch,&width,&height,&bmp);
  // returns char width
  return width;
}


// Returns the width of a string in pixels with current font settings.
int toled_strwidth(const char *s) {
  int width = 0;
  while( *s ) {
    width += toled_charwidth(*s++); 
    if( *s ) width += toled_kern; // kern, except for last char
  }
  return width;
}


// Left (align=-1), center (align=0) or right (align=1) draw string s in a 
// box of width pixels. Box starts at the cursor.
void toled_str(const char *s, int width, int align) {
  int remainingspace = width - toled_strwidth(s);
  if( align==0 ) {
    toled_x += remainingspace/2;
  } else if( align>0 ) {
    toled_x += remainingspace;
  } else {
    // no shift of toled_x
  }
  toled_str(s);
}


// Font test strings
//   " !\"#$%&'()*+,-./0123456789:;<=>?"
//   "@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^_"
//   "`abcdefghijklmnopqrstuvwxyz{|}~"
