/*
 * File:   main.c
 * 
 * Chapter 3: Graphic Resources - Splashscreen demo
 * 
 */

#include "xc.h"
#include "system.h"
#include "gfx/gfx.h"

#include "images/fingerprint.h"
//#include "fonts/arimo22.h"
#include "fonts/gentium27.h"


int main( void )
{
    int width;
    char s[] = "Fonts and Bitmaps";

    // 1. init
    SYSTEM_BoardInitialize();
    GFX_Initialize();               // init graphics library
    GFX_ColorSet( WHITE);           // set background color
    GFX_ScreenClear();              // clear display contents
    DisplayBacklightOn();           // turn on the backlight

    // 2. display centered banner
    GFX_ColorSet( BRIGHTRED);           // set color
//    GFX_FontSet( (void*) &Arimo_Regular_22);
    GFX_FontSet( (void*) &Gentium27);
//    width = GFX_TextStringWidthGet( s, (void*) &Arimo_Regular_22);
    width = GFX_TextStringWidthGet( s, (void*) &Gentium27);
    GFX_TextStringDraw( (GFX_MaxXGet()-width)/2 , 10, s, 0);

    // 3. display centered bitmap
    width = GFX_ImageWidthGet( (void*) &fingerprint);
    GFX_ImageDraw( (GFX_MaxXGet() - width)/2, 60, (void*) &fingerprint);

//    // capture screen
//    FSInit();
//    ScreenCapture( "3-SPLASH.SCR");

    // main loop
    while( 1)
    {

    } // main loop
}
