// aoui32-oled.ino - demo of the UI elements on the OSP32 board, button changes OLED
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
#include <aoui32.h>

/*
DESCRIPTION
This demo demonstrates the OLED on the OSp32 board. Button A controls the 
OLED contents, and buttons X and Y control the green and red signaling LED. 
This demo also has a splash screen.

HARDWARE
The demo runs on the OSP32 board (uses the OLED).
In Arduino select board "ESP32S3 Dev Module".

BEHAVIOR
When app starts, it shows a splash screen with "aoui32-oled.ino" and its 
version number (0.3.7). When app is running, it shows app name "AOUI32 demo" 
and the labels for the X ("green") and Y ("red") button.
While X is pressed the green/ok LED is on, while Y is pressed the red/err
LED is on. While A is pressed the OLED shows a long message.

OUTPUT
Welcome to aoui32-oled.ino
version: ui32 0.3.7
ui32: init
*/


int count;


void setup() {
  Serial.begin(115200);
  Serial.printf("\n\nWelcome to aoui32-oled.ino\n");
  Serial.printf("version: ui32 %s\n", AOUI32_VERSION );

  aoui32_init();
  Serial.printf("\n");

  aoui32_oled_splash("aoui32-oled.ino", AOUI32_VERSION );
  delay(3000);

  aoui32_oled_state("AOUI32 demo","green","red");
}


void loop() {
  aoui32_but_scan();

  if( aoui32_but_wentdown(AOUI32_BUT_A) ) aoui32_oled_msgf("Message %d: the (A) button is being pressed, please release it again.", ++count );
  if( aoui32_but_wentdown(AOUI32_BUT_X) ) aoui32_led_on( AOUI32_LED_GRN );
  if( aoui32_but_wentdown(AOUI32_BUT_Y) ) aoui32_led_on( AOUI32_LED_RED );

  if( aoui32_but_wentup  (AOUI32_BUT_A) ) aoui32_oled_state("AOUI32 demo","green","red");
  if( aoui32_but_wentup  (AOUI32_BUT_X) ) aoui32_led_off( AOUI32_LED_GRN );
  if( aoui32_but_wentup  (AOUI32_BUT_Y) ) aoui32_led_off( AOUI32_LED_RED );
}

