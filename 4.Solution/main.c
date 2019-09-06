/*
 * Project: 4-Solution - 15 Tiles
 * File:    main.c
 *
 * Requires: MLA 1312
 */
#include "system.h"
#include "tty.h"
#include "grid.h"
#include "stdlib.h"

#include "fonts/droid36.h"
#include "images/parrot.h"    // load tiles from bitmaps

uint16_t sx = (GFX_MaxXGet()+1)/4;
uint16_t sy = (GFX_MaxYGet()+1)/4;

// define a matrix to keep the position of each tile
uint8_t m[4][4];
uint8_t style=0;

const void * images[] = {
    &parrot_0_0, &parrot_1_0, &parrot_2_0, &parrot_3_0,
    &parrot_0_1, &parrot_1_1, &parrot_2_1, &parrot_3_1,
    &parrot_0_2, &parrot_1_2, &parrot_2_2, &parrot_3_2,
    &parrot_0_3, &parrot_1_3, &parrot_2_3, NULL
};


void TilePaint( uint16_t tx, uint16_t ty)
{
    uint16_t q = m[tx][ty];     // identify the tile
    uint16_t y = ty * sy;       // compute the vertical coordinate
    uint16_t x = tx * sx;       // compute the horiz coordinate
    uint16_t r = 8;             // rounded rectangle radius
    char s[3];

    if ( q < 15)
    {
        if ( style)
        {
            GFX_ImageDraw( x, y, (void*)images[ q]);
        }
        else
        {// color the tile, use the gradient functions
            GFX_GradientColorSet( RED, BRIGHTRED);
            GFX_FillStyleSet( GFX_FILL_STYLE_GRADIENT_DOUBLE_HOR);
            GFX_RectangleRoundFillDraw( x+r, y+r, x+sx-r, y+sy-r, r);
            GFX_ColorSet( DARKGRAY);
            GFX_RectangleRoundDraw( x+r, y+r, x+sx-r, y+sy-r, r);

            // add the number, use the boxed text functions
            itoa( s, q+1, 10);
            GFX_FontSet( (void*)&DroidSans_Bold_36);
            GFX_ColorSet( WHITE);
            GFX_TextStringBoxDraw( x, y, sx, sy, s, 0, GFX_ALIGN_CENTER);
        }
    }
    else
    {   // erase tile
        GFX_ColorSet( WHITE);
        GFX_BarDraw( x, y, x+sx-1, y+sy-1);
    }
}


void TilesSwap( uint8_t x, uint8_t y, int8_t dx, int8_t dy)
{
    // move the tile in the empty space
    m[x+dx][y+dy] = m[x][y];
    TilePaint( x+dx, y+dy);

    m[x][y] = 15;
    TilePaint( x, y);
} // swapTiles


int main( void )
{
    uint8_t   x, y, k;
    code_t    q;
    
    // 1. init board
    SYSTEM_BoardInitialize();

    // 2. init touch module (do not use NVM to store calibration data)
    Grid_Init( 4, 4);

    // 3. init the graphics
    TTY_Init();
    TTY_ColorSet( RED);
    TTY_BackgroundSet( WHITE);
    TTY_Clear();
    DisplayBacklightOn();

    // 4. splash screen
    TTY_StringCenter( -3, "15 Tiles Game");
    TTY_StringCenter( -1, "Select to start");
    style = 1; m[1][2]=11; TilePaint( 1, 2); // images
    style = 0; m[2][2]=14; TilePaint( 2, 2); // numbers
    q = Grid_ButtonGet();
    style = ( q.x >1) ? 0 : 1;
    TTY_Clear();

    // 5. init the 4x4 matrix
    k = 0;
    for( y=0; y<4; y++)
        for( x=0; x<4; x++)
        {
            m[x][y]= k++;
            TilePaint( x, y);
        }

    // 6. scramble the tiles
    // ...

    // 7. main loop
    while( 1 )
    {
        q =  Grid_ButtonGet();
        x = q.x;
        y = q.y;

        // check if near the 0 tile
        if ((x>0) && ( m[x-1][y] == 15))
            TilesSwap( x, y, -1, 0);
        if ((x<3) && ( m[x+1][y] == 15))
            TilesSwap( x, y, +1, 0);
        if ((y>0) && ( m[x][y-1] == 15))
            TilesSwap( x, y, 0, -1);
        if ((y<3) && ( m[x][y+1] == 15))
            TilesSwap( x, y, 0, +1);


    } // main loop
} // main
