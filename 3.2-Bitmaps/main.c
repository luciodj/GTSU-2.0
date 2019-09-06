/*
 * File:   main.c
 * 
 * Chapter 3: Graphic Resources - Color Splashscreen
 * 
 */
#include "system.h"
#include "gfx/gfx.h"

#include "images/fingerprint.h"
#include "images/flower.h"
#include "fonts/arimo22.h"


int main( void )
{
    int w1, w2, pad;
    char s[] = "Fonts and Bitmaps";

    // 1. init
    SYSTEM_BoardInitialize();
    GFX_Initialize();               // init graphics library
    GFX_ColorSet( WHITE);           // set background color
    GFX_ScreenClear();              // clear display contents
    DisplayBacklightOn();           // turn on the backlight

    // 2. display centered banner
    GFX_ColorSet( BRIGHTRED);           // set color
    GFX_FontSet( (void*) &Arimo_Regular_22);
    w1 = GFX_TextStringWidthGet( s, (void*) &Arimo_Regular_22);
    GFX_TextStringDraw( (GFX_MaxXGet()-w1)/2 , 10, s, 0);

    // 3. display centered bitmaps
    w1 = GFX_ImageWidthGet( (void*) &fingerprint);
    w2 = GFX_ImageWidthGet( (void*) &flower);
    pad = (GFX_MaxXGet() - w1 - w2)/3;
    GFX_ImageDraw( pad, 60, (void*) &fingerprint);
    GFX_ImageDraw( pad + w1 + pad, 60, (void*) &flower);

// capture screen
//    FSInit();
//    ScreenCapture( "3-SPLASH.SCR");

    // main loop
    while( 1)
    {

    } // main loop
}

