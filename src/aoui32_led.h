// aoui32_led.h - drivers for the signaling LEDs (UI) on the OSP32 board.
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
#ifndef _AOUI32_LED_H_
#define _AOUI32_LED_H_


// LED pins for the OSP32 board.
#define AOUI32_LED_GRN_PIN  7
#define AOUI32_LED_RED_PIN 15


// Bit mask for the green signaling LED.
#define AOUI32_LED_GRN   (1<<0)
// Bit mask for the red signaling LED.
#define AOUI32_LED_RED   (1<<1)
// Bit mask for all signaling LED.
#define AOUI32_LED_ALL   ( AOUI32_LED_GRN | AOUI32_LED_RED )


// This function sets the LEDs in mask `leds` to "on".
void aoui32_led_on    (int leds);
// This function sets the LEDs in mask `leds` to "off".
void aoui32_led_off   (int leds);
// This function toggles the LEDs in mask `leds`.
void aoui32_led_toggle(int leds);


// Initializes the signaling LED pins (do not call, is called by aoui32 init).
void aoui32_led_init(int pin_grn=AOUI32_LED_GRN_PIN, int pin_red=AOUI32_LED_RED_PIN); 


#endif
