/*
 * Project: 3.Solution
 * 
 * File:   main.c
 *
 * Requires: MLA 1306
 */

#include "system.h"
#include "tty.h"

#include <stdio.h>          // sprintf
#include <stdarg.h>         // var args

void Log( const char *fmt, ...)
{
    char s[128];
    va_list argp;

    va_start( argp, fmt);
    vsprintf( s, fmt, argp);
    TTY_StringPut( s);
    TTY_CharPut( '\n');
    va_end( argp);
}


int main( void )
{
    int i;

    // init the graphics
    SYSTEM_BoardInitialize();
    TTY_Init();
    DisplayBacklightOn();

    // main loop
    while( 1 )
    {
        Log( "Hello World #%04x", i++);
//        printf( "Hello World #%04x ", i++);
        //DelayMs(20);

    } // main loop
} // main



