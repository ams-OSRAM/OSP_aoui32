// aoui32_led.cpp - drivers for the signaling LEDs (UI) on the OSP32 board.
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
#include <Arduino.h> // pinMode
#include <aoui32.h>  // own


// Which pins drive the green and red LED.
static int aoui32_led_grn_pin;
static int aoui32_led_red_pin;


/*!
    @brief  Initializes the signaling LED pins.
    @param  pin_grn
            The pin number for the green LED - assumed high active.
    @param  pin_red
            The pin number for the red LED - assumed high active.
    @note   All pins are hardwired matching the OSP32 board.
*/
void aoui32_led_init(int pin_grn, int pin_red) {
  // Record pins
  aoui32_led_grn_pin= pin_grn;
  aoui32_led_red_pin= pin_red;
  // Switch LEDs off (high active)
  digitalWrite(aoui32_led_grn_pin, LOW );
  digitalWrite(aoui32_led_red_pin, LOW );
  // Configure the GPIO pins
  pinMode( aoui32_led_grn_pin, OUTPUT );
  pinMode( aoui32_led_red_pin, OUTPUT );
}


/*!
    @brief  This function sets the LEDs in mask `leds` to "on".
    @param  leds
            A mask, formed by OR-ing AOUI32_LED_... macros.
*/
void aoui32_led_on(int leds) {
  if( leds & AOUI32_LED_GRN ) digitalWrite(aoui32_led_grn_pin, HIGH);
  if( leds & AOUI32_LED_RED ) digitalWrite(aoui32_led_red_pin, HIGH);
}


/*!
    @brief  This function sets the LEDs in mask `leds` to "off".
    @param  leds
            A mask, formed by OR-ing AOUI32_LED_... macros.
*/
void aoui32_led_off(int leds) {
  if( leds & AOUI32_LED_GRN ) digitalWrite(aoui32_led_grn_pin, LOW);
  if( leds & AOUI32_LED_RED ) digitalWrite(aoui32_led_red_pin, LOW);
}


/*!
    @brief  This function toggles the LEDs in mask `leds`.
    @param  leds
            A mask, formed by OR-ing AOUI32_LED_... macros.
*/
void aoui32_led_toggle(int leds) {
  if( leds & AOUI32_LED_GRN ) digitalWrite(aoui32_led_grn_pin, ! digitalRead(aoui32_led_grn_pin) );
  if( leds & AOUI32_LED_RED ) digitalWrite(aoui32_led_red_pin, ! digitalRead(aoui32_led_red_pin) );
}
