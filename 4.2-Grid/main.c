/*
 * Project: 4.2-Grid
 * File:    main.c
 *
 * Requires: MLA 1312
 */

#include "system.h"
#include "grid.h"
#include "tty.h"

int main( void )
{
    uint16_t sx = (GFX_MaxXGet()+1)/4;
    uint16_t sy = (GFX_MaxYGet()+1)/4;

    SYSTEM_BoardInitialize();
    
    // 3. init the graphics
    TTY_Init();
    Grid_Init( 4, 4);
    DisplayBacklightOn();

    // 4. splash screen
    TTY_StringCenter( -1, "Grid Demo");
    TTY_StringCenter( +1, "Tap to start");
    while( TouchGetX() < 0);    // wait for tap
    while( TouchGetX() > 0);    // wait for release
    TTY_Clear();

    // 5. main loop
    while( 1 )
    {
        int x, y;
        code_t q;

        // draw the grid
        GFX_ColorSet( LIGHTGRAY);
        GFX_LineStyleSet( GFX_LINE_STYLE_THIN_DASHED);
        for( x=1; x<4; x++)
            GFX_LineDraw(x*sx, 0, x*sx, GFX_MaxYGet() );
        for( y=1; y<4; y++)
            GFX_LineDraw( 0, y*sy, GFX_MaxXGet(), y*sy);

        // wait for touch on the grid
        q = Grid_ButtonGet();

        // remove previous images
        GFX_ColorSet( BLACK);
        GFX_ScreenClear();

        // choose color based on how long the pressure has been applied
        GFX_ColorSet( ( q.option ) ?  BRIGHTRED : GREEN);

        // position a filled tile on the grid
        y = sy * q.y;
        x = sx * q.x;
        GFX_RectangleRoundFillDraw( x+5, y+5, x + sx-5, y + sy-5 , 5);
//        if ( x >160)
//        {
//            uMBInit(); FSInit();
//            ScreenCapture( "4-2-GRID.SCR");
//        }

    } // main loop
} // main



