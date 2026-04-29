// aoui32_led.h - drivers for the 128x32 pixel OLED (UI) on the OSP32 board.
/*****************************************************************************
 * Copyright 2024-2026 by ams OSRAM AG                                       *
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
#ifndef _AOUI32_OLED_H_
#define _AOUI32_OLED_H_


// Give access to underlying OLED driver
#include <toled/toled.h> 


// OLED settings for the OSP32 board.
#define AOUI32_OLED_SDA_PIN   8
#define AOUI32_OLED_SCL_PIN  18
#define AOUI32_OLED_I2C_FREQ (1000*1000) // Works for OLED, too high for other I2C devices on the bus


// Replaces the OLED screen with a full screen rectangle with the passed `word` shown in a big font.
void aoui32_oled_word(const char * word);
// Replaces the OLED screen with a full screen rectangle with the passed sentence `msg` shown in a small font.
void aoui32_oled_msg(const char * msg);
// Replaces the OLED screen with a full screen rectangle with the passed formatted message (as printf) shown in a small font.
void aoui32_oled_msgf(const char * fmt, ... );
// Replaces the OLED screen with an "app status screen" showing the app `name`, and what the buttons do.
void aoui32_oled_state(const char * astr, const char * xstr, const char * ystr, const char * albl="A", const char * xlbl="x", const char * ylbl="y");
// Replaces the OLED screen with a splash screen showing an "OSP logo", the executable name, and the executable version.
void aoui32_oled_splash(const char * execname, const char * version);


// Initializes the I2C bus pins and speed, and configures the OLED (do not call, is called by aoui32 init).
void aoui32_oled_init(int sda=AOUI32_OLED_SDA_PIN, int scl=AOUI32_OLED_SCL_PIN, uint32_t freq=AOUI32_OLED_I2C_FREQ);


#endif
