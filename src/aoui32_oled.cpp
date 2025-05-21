// aoui32_oled.cpp - drivers for the 128x32 OLED (UI) on the OSP32 board.
/*****************************************************************************
 * Copyright 2024,2025 by ams OSRAM AG                                       *
 * All rights are reserved.                                                  *
 *                                                                           *
 * IMPORTANT - PLEASE READ CAREFULLY BEFORE COPYING, INSTALLING OR USING     *
 * THE SOFTWARE.                                                             *
 *                                                                           *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS       *
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT         *
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS         *
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT  *
 * OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,     *
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT          *
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,     *
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY     *
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT       *
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE     *
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.      *
 *****************************************************************************/
#include <Arduino.h> // pinMode
#include <Wire.h>    // Wire.begin()
#include <aoui32.h>  // own


/*!
    @brief  Replaces the OLED screen with a full screen rectangle 
            with the passed message shown in a small font.
    @param  msg
            The message to shows.
    @note   This is intended to show an error message.
    @note   See also `aoui32_oled_msgf()`.
*/
void aoui32_oled_msg(const char * msg) {
  toled_clear();

  // Have the message in the (2,2)-(125,29) box
  toled_font(toled_font_sans8, TOLED_COL_WHITE, 1, 2); // margin 2 to fit frame
  toled_cursor(2,2);
  toled_str(msg);

  // Frame it
  toled_openrect(0, 0, 127, 31 );

  toled_commit();
}


/*!
    @brief  Replaces the OLED screen with a full screen rectangle 
            with the passed formatted message shown in a small font.
    @brief  Formatted print to the quad 7-segment display.
    @param  fmt, ...
            Formatted message as in printf()
    @note   This is intended to show an error message.
*/
void aoui32_oled_msgf(const char * fmt, ... ) {
  char str[96]; 
  va_list args;
  va_start(args,fmt);
  vsnprintf(str,sizeof str,fmt,args);
  va_end(args);
  aoui32_oled_msg(str);
}


/*!
    @brief  Replaces the OLED screen with an "app status screen" showing 
            the app name, and what the buttons do.
    @param  name
            The name of the running application (shown top, in big font).
    @param  xlbl
            The function of the X button (shown left, in small font).
    @param  ylbl
            The function of the Y button (shown right, in small font).
    @note   One firmware image ("executable") has multiple applications (modes).
    @note   This is intended to show the user what application is running and how
            to use the buttons.
    @note   The three texts are tagged on the OLED with a tiny 'A', 'x', and 'y'.
*/
void aoui32_oled_state(const char * name, const char * xlbl, const char * ylbl) {
  toled_clear();

  // Boxes for x and y labels
  toled_fillrect(0, 20, 62, 31 );
  toled_fillrect(64, 20, 127, 31 );

  // Name bigger
  toled_font(toled_font_sans14 );
  toled_cursor(0,0);
  toled_str(name,128);

  // X and y label text smaller (and reverse video)
  toled_font(toled_font_sans8, TOLED_COL_BLACK );
  toled_cursor(0,21);
  toled_str(xlbl, 64);
  toled_cursor(64,21);
  toled_str(ylbl,64);

  // Name label
  toled_font(toled_font_sans5 );
  toled_cursor(0,0);
  toled_str("A");
  
  // X and Y labels
  toled_font(toled_font_sans5, TOLED_COL_BLACK );
  toled_cursor(1,20);
  toled_str("x");
  toled_cursor(65,20);
  toled_str("y");

  toled_commit();
}


// Made with aoui32\extra\oled-img.xlsx
static const uint32_t aoui32_oled_osp8logo[] = {
  0b00001111111111111111111111110000,
  0b00111111111111111111111111111100,
  0b01111111111111111111111111111110,
  0b01111111111111111111111111111110,
  0b11111111111111111111111111111111,
  0b11111111111111111111111111111111,
  0b11111111111111111111111111111111,
  0b11111111111111111111111111111111,
  0b11111111100000000000000111111111,
  0b11111111000000000000000011111111,
  0b11111111000000000000000011111111,
  0b11111111100000000000000111111111,
  0b11111111111111111111111111111111,
  0b11111111111111111111111111111111,
  0b11111111111111111111111111111111,
  0b11111111111111111111111111111111,
  0b01111111111111111111111111111110,
  0b01111111111111111111111111111110,
  0b00111111111111111111111111111100,
  0b00001111111111111111111111110000,
  0b00000000000000000000000000000000,
  0b00000000000000000000000000000000,
  0b11111111000000000111111111110000,
  0b11111111000000111111111111111100,
  0b11111111000001111111111111111110,
  0b11111111000001111111111111111110,
  0b11111111000011111111111111111111,
  0b11111111000011111111111111111111,
  0b11111111000011111111111111111111,
  0b11111111000011111111111111111111,
  0b11111111000011111111100111111111,
  0b11111111000011111111000011111111,
  0b11111111000011111111000011111111,
  0b11111111000011111111000011111111,
  0b11111111100111111111000011111111,
  0b11111111111111111111000011111111,
  0b11111111111111111111000011111111,
  0b11111111111111111111000011111111,
  0b11111111111111111111000011111111,
  0b01111111111111111110000011111111,
  0b01111111111111111110000011111111,
  0b00111111111111111100000011111111,
  0b00001111111111110000000011111111,
  0b00000000000000000000000000000000,
  0b00000000000000000000000000000000,
  0b11111111111111111111111111111111,
  0b11111111111111111111111111111111,
  0b11111111111111111111111111111111,
  0b11111111111111111111111111111111,
  0b11111111111111111111111111111111,
  0b11111111111111111111111111111111,
  0b11111111111111111111111111111111,
  0b11111111111111111111111111111111,
  0b00000000000011111111100111111111,
  0b00000000000011111111000011111111,
  0b00000000000011111111000011111111,
  0b00000000000011111111000011111111,
  0b00000000000011111111100111111111,
  0b00000000000011111111111111111111,
  0b00000000000011111111111111111111,
  0b00000000000011111111111111111111,
  0b00000000000011111111111111111111,
  0b00000000000001111111111111111110,
  0b00000000000001111111111111111110,
  0b00000000000000111111111111111100,
  0b00000000000000001111111111110000,
};
#define AOUI32_OLED_OSP8LOGO_WIDTH (sizeof(aoui32_oled_osp8logo)/sizeof(uint32_t))


/*!
    @brief  Replaces the OLED screen with a splash screen showing an "OSP logo", 
            the executable name, and the executable version.
    @param  execname
            The name of the executable (the firmware image).
    @param  version
            The version of the executable.
    @note   This is intended to show at startup, which firmware is flashed.
*/
void aoui32_oled_splash(const char * execname, const char * version) {
  toled_clear();

  int logowidth = AOUI32_OLED_OSP8LOGO_WIDTH;
  const uint32_t * data = aoui32_oled_osp8logo;
  for( int y=0; y<32; y++ ) {
    for( int x=0; x<logowidth; x++ ) {
      if( data[x] & (1<<y) ) toled_pixel(x,y);
    }
  }

  // `version` in medium font flush right
  toled_font(toled_font_sans14 );
  toled_cursor(0, 0 );
  toled_str(version,128,1); // right align over remaining space
 
  // `exec` in small font flush right
  toled_font(toled_font_sans8 );
  toled_cursor(0,32 -8-2);
  toled_str(execname,128,1); // right align over remaining space
  
  toled_commit();
}



// OLED pins are hardwired matching the OSP32 board.
#define AOUI32_OLED_SDA_PIN   8
#define AOUI32_OLED_SCL_PIN  18
#define AOUI32_OLED_I2C_FREQ (1000*1000) // Works for OLED, too high for other I2C devices on the bus


/*!
    @brief  Initializes the I2C bus pins and speed, and configures the OLED.
    @note   All pins are hardwired matching the OSP32 board.
    @note   Do not attach other I2C devices to the same bus, 
            they probably can not handle the bus speed.
*/
void aoui32_oled_init() {
  Wire.begin(AOUI32_OLED_SDA_PIN,AOUI32_OLED_SCL_PIN);
  Wire.setClock(AOUI32_OLED_I2C_FREQ); 
  toled_init();
}
