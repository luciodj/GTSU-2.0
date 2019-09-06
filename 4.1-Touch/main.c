/*
 * Project: 4.1 Touch
 * File:    main.c
 *
 * Requires: MLA 1312
 */

#include "system.h"
#include "tty.h"

int main( void )
{
    // 1. init the board
    SYSTEM_BoardInitialize();

    // 4. init the terminal emulator
    TTY_Init();
    DisplayBacklightOn();
    
    // 4. splash screen
    TTY_Clear();
    TTY_StringCenter( -1, "Splash Screen");
    TTY_StringCenter(  1, "Tap to continue");
    while ( TouchGetX()<0);
    TTY_Clear();
    
    // 5. main loop
    while( 1 )
    {

        if (( TouchGetX() != -1) && ( TouchGetY() != -1))
        {
            AT( 1,_MAX_Y/2);
            printf( "Touched at: %4d, %4d", TouchGetX(), TouchGetY());

//            if ( TouchGetX() > 160)
//            {
//                uMBInit(); FSInit();
//                ScreenCapture( "4-1-TERM.SCR");
//            }
        }

    } // main loop
} // main



