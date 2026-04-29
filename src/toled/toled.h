// toled.h - Tiny OLED driver (only 128x32, only I2C, only for ESP32)
#ifndef _TOLED_H_
#define _TOLED_H_


#include <stdint.h>


#define TOLED_VERSION  "0.7.1"
// 20260116 0.7.1 Fixed bug for short verline's and fillrect's
// 20260108 0.7.0 Added mono5 font, 
//                digits sans8 now all 5 wide (`1`), 
//                digits sans10 now all 6 wide (`1`), 
//                digits sans12 now all 7 wide (`1` and `4`), 
//                sans5 `=` and `a` changed width 5 to 4, 
//                code comments improved; 
//                removed vertical wrap around;
//                added more font examples plus pictures in extras.
// 20240611 0.6.3 bug fix: undid stricter text wrapping
// 20240607 0.6.2 out of bounds writes ignored.
// 20240417 0.6.1 updates example toled-speed to measure speed of text.
// 20200416 0.6.0 added sans5.
// 20200415 0.5.0 added margin for wrap-around.
// 20200414 0.4.0 and later supports (screen) wrap-around for (long) strings.
// 20200413 0.3.0 and later supports text using 4 font sizes.


#define TOLED_WIDTH    128
#define TOLED_HEIGHT    32


// "Colors", i.e. drawing mode
#define TOLED_COL_BLACK 0   // Draw black pixels (off)
#define TOLED_COL_WHITE 1   // Draw white pixels (on)
#define TOLED_COL_FLIP  2   // Draw pixels by inverting


// toled management
void toled_init(void);
void toled_clear(void);
void toled_commit(void);

// toled pixels, lines, rectangles
void toled_pixel(int x, int y, int col=TOLED_COL_WHITE );
void toled_verline(int x, int y0, int y1, int col=TOLED_COL_WHITE );
void toled_horline(int x0, int x1, int y, int col=TOLED_COL_WHITE );
void toled_fillrect(int x0, int y0, int x1, int y1, int col=TOLED_COL_WHITE );
void toled_openrect(int x0, int y0, int x1, int y1, int col=TOLED_COL_WHITE );

// toled text
typedef enum toled_font_e {
  toled_font_mono5, 
  toled_font_sans5, 
  toled_font_sans8, 
  toled_font_sans10, 
  toled_font_sans12, 
  toled_font_sans14, 
} toled_font_t;

void toled_font(toled_font_t font, int col=TOLED_COL_WHITE, int kern=1, int margin=0 );
void toled_cursor(int x, int y);

void toled_char(char ch);
void toled_str(const char *s);

int toled_charwidth(char ch);
int toled_strwidth(const char *s);

void toled_str(const char *s, int width, int align=0);

#endif
