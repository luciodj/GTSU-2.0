/*
 * Project: 6.1-Sound_Test
 *
 * File:    main.c
 */

#include "system.h"
#include "tty.h"
#include "vs1053.h"


int main( void )
{
    // initializations
    SYSTEM_BoardInitialize();
    TTY_Init();
    
    // title screen
    TTY_Clear();
    TTY_StringCenter( 0, "Sine Test");
    DisplayBacklightOn();

    while( 1 )
    {
        CODEC_Init( CODEC_TEST);        // TEST MODES enabled

        CODEC_SineTest( 0x44);          // 32ksmps, 1KHz out, 500ms pulse
        Tick_DelayMs( 500);

        CODEC_TestExit();
        Tick_DelayMs( 500);

    } // main loop
}


