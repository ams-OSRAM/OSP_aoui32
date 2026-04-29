// aoui32_but.h - drivers for the buttons (UI) on the OSP32 board.
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
#ifndef _AOUI32_BUT_H_
#define _AOUI32_BUT_H_


// Button pins for the OSP32 board.
#define AOUI32_BUT_A_PIN  0
#define AOUI32_BUT_X_PIN  17
#define AOUI32_BUT_Y_PIN  16


// Bit mask for the button labeled 'A'.
#define AOUI32_BUT_A   (1<<0)
// Bit mask for the button labeled 'X'.
#define AOUI32_BUT_X   (1<<1)
// Bit mask for the button labeled 'Y'.
#define AOUI32_BUT_Y   (1<<2)
// Bit mask for all buttons.
#define AOUI32_BUT_ALL ( AOUI32_BUT_A | AOUI32_BUT_X | AOUI32_BUT_Y )


// This function scans all buttons for their state (up, down) and records that in a global variable. 
void aoui32_but_scan();
// Returns which of the passed `buts` were up (released) during the last aoui32_but_scan() call.
int  aoui32_but_isup    (int buts);
// Returns which of the passed `buts` were down (depressed) during the last aoui32_but_scan() call.
int  aoui32_but_isdown  (int buts);
// Returns which of the passed `buts` were down (depressed) during the last aoui32_but_scan() call, but where up (released) during the call to aoui32_but_scan() before that; the button "went down".
int  aoui32_but_wentdown(int buts);
// Returns which of the passed `buts` were up (released) during the last aoui32_but_scan() call, but where down (depressed) during the call to aoui32_but_scan() before that; the button "went up".
int  aoui32_but_wentup  (int buts);

// Initializes the button pins (do not call, is called by aoui32 init).
void aoui32_but_init(int pin_a=AOUI32_BUT_A_PIN, int pin_x=AOUI32_BUT_X_PIN, int pin_y=AOUI32_BUT_Y_PIN);


#endif
