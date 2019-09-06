/*
 * File:   main.c
 *
 * Project: 2-HelloMLA
 */
#include "xc.h"
#include "system.h"
#include "gfx/gfx.h"

int main( void )
{
    int i;

    // 1. Board initialization
    SYSTEM_BoardInitialize();
    GFX_Initialize();

    // 2. Application initialization
    GFX_ColorSet( GREEN);
    GFX_ScreenClear();
    DisplayBacklightOn();

    // 3. testing the primitives
    GFX_ColorSet( BLACK);
    for( i=0; i<100; i+=10)
    {
        GFX_LinePositionSet( 10, 10);
        GFX_LineToDraw( 100, 10+i);
        GFX_CircleDraw( 20+i, 220-i, 10+i);
    }

    GFX_ColorSet( BRIGHTRED);
    GFX_BarDraw(120, 10, 170, 60);
    GFX_RectangleDraw( 120, 10, 200, 90);

    GFX_ColorSet( BLUE);
    GFX_RectangleRoundFillDraw( 220, 20, 250, 50, 10);
    GFX_RectangleRoundDraw( 220, 20, 310, 110, 10);

    GFX_ColorSet( ORANGE);
    GFX_CircleFillDraw( 270, 190, 40);

    
    // 5. main loop
    while( 1);

} // main
