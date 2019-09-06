/*
 * Project: 2.Solution
 * File:    main.c
 *
 */

#include "system.h"
#include "fonts/arimo22.h"


/**
 * @brief  center text horizontally
 *
 * @param y             vertical coordinate
 * @param color         color used for text
 * @param s             string to be centered
 */
void CenteredText( uint16_t y, uint16_t color, char* s)
{
    uint16_t margin;
    uint16_t width = GFX_TextStringWidthGet( s, GFX_FontGet());

    // check if too large to fit
    if ( width > GFX_MaxXGet())
        margin = 0;                         // use no margin, clip the right side
    else
    // compute margin to center
        margin = (GFX_MaxXGet() - width) / 2;   // split margin equally on both sides

    // set the color
    GFX_ColorSet( color);

    // print string on screen
    GFX_TextStringDraw( margin, y, s, 0);

} // Centered Text


int main( void )
{
    unsigned height;

    // 1. init
    SYSTEM_BoardInitialize();
    GFX_Initialize();
    DisplayBacklightOn();
    GFX_FontSet( (void*) &Arimo_Regular_22);

    // 2. Centered Title vertically
    height = GFX_TextStringHeightGet( GFX_FontGet());
    CenteredText( GFX_MaxYGet()/2-height, BRIGHTRED, "Chapter 2: Solution");

    // main loop
    while( 1)
    {

    } // main loop
}