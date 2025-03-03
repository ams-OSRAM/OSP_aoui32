// aoui32_but.cpp - drivers for the buttons (UI) on the OSP32 board.
/*****************************************************************************
 * Copyright 2024 by ams OSRAM AG                                            *
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


// Minimum amount of time between scans, to prevent picking up contact bounce
#define AOUI32_BUT_BOUNCE_MS 50


// Button pins are hardwired matching the OSP32 board.
#define AOUI32_BUT_A_PIN  0
#define AOUI32_BUT_X_PIN  17
#define AOUI32_BUT_Y_PIN  16


static unsigned aoui32_but_prvstate; // button state of one-but-last scan()
static unsigned aoui32_but_curstate; // button state of last scan()
static uint32_t aoui32_but_lastscan; // time of last scan()


/*!
    @brief  Initializes the button pins.
    @note   All pins are hardwired matching the OSP32 board.
*/
void aoui32_but_init() {
  // Configure the GPIO pins
  pinMode( AOUI32_BUT_A_PIN, INPUT_PULLUP );
  pinMode( AOUI32_BUT_X_PIN, INPUT_PULLUP );
  pinMode( AOUI32_BUT_Y_PIN, INPUT_PULLUP );
  // Scan the button states, 
  aoui32_but_lastscan= millis() - AOUI32_BUT_BOUNCE_MS; // force a capture in aoui32_but_scan()
  aoui32_but_scan();
  aoui32_but_prvstate = aoui32_but_curstate;
}


/*!
    @brief  This function scans all buttons for their state (up, down) 
            and records that in a global variable. 
    @note   Call aoui32_but_scan() frequently for a responsive UI.
    @note   The previous state is also recorded. Subsequently use functions 
            aoui32_but_isup()/down(), and/or aoui32_but_wentup()/wentdown() 
            to determine which buttons are respectively went up/down.
    @note   Do not skip checking wentdown() [up] between two calls of scan().
    
            button   ----------_-_-_-_______________-_-_------------------
            scan     --+-----+-----+-----+-----+-----+-----+-----+-----+--
            isup     ---------------________________________--------------
            isdown   _______________------------------------______________
            wentdown _______________------________________________________
            wentup   _______________________________________------________
            
                                   ^     ^
            If no check between these two calls, the wentdown() is missed.
*/
void aoui32_but_scan() {
  // Copy previous state (so that only a single wentxxx happens)
  aoui32_but_prvstate = aoui32_but_curstate;

  // Skip if still in "contact bounce time"
  if( millis()-aoui32_but_lastscan < AOUI32_BUT_BOUNCE_MS ) return;

  // Get all GPIO ports in one read
  uint32_t buts=REG_READ(GPIO_IN_REG) ;
  aoui32_but_curstate = AOUI32_BUT_ALL;
  // All three buttons are low active
  if( buts & (1<<AOUI32_BUT_A_PIN) ) aoui32_but_curstate ^= AOUI32_BUT_A;
  if( buts & (1<<AOUI32_BUT_X_PIN) ) aoui32_but_curstate ^= AOUI32_BUT_X;
  if( buts & (1<<AOUI32_BUT_Y_PIN) ) aoui32_but_curstate ^= AOUI32_BUT_Y;
  // Record new capture time
  aoui32_but_lastscan= millis();
}


/*!
    @brief  Returns which of the passed buttons were up (released)
            during the last aoui32_but_scan() call.
    @param  buts
            A mask, formed by OR-ing AOUI32_BUT_... macros.
    @return Returns a mask, subset of buts, 
            with a 1 for buttons that were up.
    @note   Must aoui32_but_scan() before using this function.
*/
int aoui32_but_isup( int buts ) {
  return ~aoui32_but_curstate & buts;
}


/*!
    @brief  Returns which of the passed buttons were down (depressed)
            during the last aoui32_but_scan() call.
    @param  buts
            A mask, formed by OR-ing AOUI32_BUT_... macros.
    @return Returns a mask, subset of buts, 
            with a 1 for buttons that were down.
    @note   Must aoui32_but_scan() before using this function.
*/
int aoui32_but_isdown( int buts ) {
  return  aoui32_but_curstate & buts;
}


/*!
    @brief  Returns which of the passed buttons were down (depressed)
            during the last aoui32_but_scan() call, but where up 
            (released) during the call to aoui32_but_scan() before that; 
            the button "went down".
    @param  buts
            A mask, formed by OR-ing AOUI32_BUT_... macros.
    @return Returns a mask, subset of buts, 
            with a 1 for buttons that went down.
    @note   Must aoui32_but_scan() before using this function.
*/
int aoui32_but_wentdown( int buts ) {
  return (aoui32_but_prvstate ^ aoui32_but_curstate) & aoui32_but_curstate & buts;
}


/*!
    @brief  Returns which of the passed buttons were up (released)
            during the last aoui32_but_scan() call, but where down 
            (depressed) during the call to aoui32_but_scan() before that; 
            the button "went up".
    @param  buts
            A mask, formed by OR-ing AOUI32_BUT_... macros.
    @return Returns a mask, subset of buts, 
            with a 1 for buttons that went up.
    @note   Must aoui32_but_scan() before using this function.
*/
int aoui32_but_wentup( int buts ) {
  return (aoui32_but_prvstate ^ aoui32_but_curstate) & ~aoui32_but_curstate & buts;
}


