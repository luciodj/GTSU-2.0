/*
 * Project: 1-Solution.X
 * File:    main.c
 */

#include <system.h>


void __RTCC_ISR  _RTCCInterrupt( void)
{
    DISPLAY_BacklightSet( 1 - DISPLAY_BacklightGet());  // toggle LED_BLED output
    RTCC_InterruptFlagClear();
}

int main( void )
{
    // 1. init I/O 
    DISPLAY_BacklightConfig();        // configure I/O as output

    // 2. Init RTCC module
    RTCC_Init();                     // init RTCC for 1/2s interrupt
    RTCC_InterruptEnable();          // enable alarm interrupts

    // 3. main loop
    while( 1)
    {
        CORE_Sleep();                // go to sleep

    } // main loop

} // main
