/*
 * Project: 1- Hello World a.k.a. Big Blinky
 *
 * File:    main.c
 */

#include "xc.h"

_CONFIG1( JTAGEN_OFF        // disable JTAG interface
        & GCP_OFF           // disable general code protection
        & GWRP_OFF          // disable flash write protection
        & ICS_PGx2          // ICSP interface (2=default)
        & FWDTEN_OFF)       // disable watchdog timer

_CONFIG2( PLL_96MHZ_ON      // enable USB PLL module
        & PLLDIV_DIV2       // 8MHz/2 = 4Mhz input to USB PLL
        & IESO_OFF          // two speed start up disabled
        & FCKSM_CSDCMD      // disable clock-switching/monitor
        & FNOSC_PRIPLL      // primary oscillator: enable PLL
        & POSCMOD_XT)       // primary oscillator: XT mode


#define DISPLAY_Backlight            _RD2
#define DISPLAY_BacklightConfig()    ( _TRISD2 = 0)

// define a 1/4 second delay given a 16MHz peripheral clock and 1:256 prescaler
#define DELAY                   16000000UL / 256 /4

int main( void )
{
    // 1. init I/O and timer
    DISPLAY_BacklightConfig();      // configure I/O as output
    T1CON = 0x8030;                 // internal clock/2 /256

    // 2. main loop
    while (1)
    {
        TMR1 = 0;                   // delay
        while ( TMR1< DELAY);

        DISPLAY_Backlight = 1 - DISPLAY_Backlight;    // toggle LED_BLED output
    } // main loop

} // main
