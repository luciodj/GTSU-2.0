/*
 * File:   TouchGrid.c
 *
 *
 * Requires: MLA 1312, TTY.c, TouchScreen.c
 */

#include "Grid.h"

static int Xside=1, Yside=1;

void Grid_Init( int x, int y)
{
    Grid_SizeSet( x, y);

} // Touch Grid Init


code_t Grid_TouchGet( void)
{   // returns 10..1F if screen pressed, 0 = none
    int x, y;
    code_t r;

    r.valid = 0;

    // 1. get the latest reading
    x = TouchGetX();  y = TouchGetY();

    // 2. if one of the two is null the other is too
    if (( x < 0) || ( y < 0))
        return r;       // return if no valid code found

    // 3. identify point on grid (4x4)
    r.y  = y / ( Yside); r.x =  x / ( Xside) ;
    r.valid = 1;
    r.option = 0;

    return r;

} // TouchGet


code_t Grid_ButtonGet( void)
{   // wait for a key pressed and debounce
    int released = 0;           // released counter
    int pressed = 0;            // pressed counter
    code_t code;                // grid code
    code_t r;                   // return value

    // 1. wait for a key pressed for at least 10 loops
    while ( pressed < 10)
    {
        code = Grid_TouchGet();
        if ( code.valid > 0) pressed++;
        else  pressed = 0;

        // wait for at least one sample time
        SYSTEM_DelayMs(1);
    }

    // 2. wait for key released for at least 10 loops
    while ( released < 10)
        {
        code = Grid_TouchGet();
        if ( code.valid > 0)
        {
            r = code;
            released = 0;       // not released yet
            pressed++;          // still pressed, keep counting
        }
        else released++;

        // wait for at least one sample time
        SYSTEM_DelayMs(1);
        }

    // 3. check if a button was pushed longer than 500ms
    if ( pressed > 500)
        r.option = 1;

    // 4. return code
    return r;

} // TouchGrid


void Grid_SizeSet( int x, int y)
{
        // define the grid dimensions
    Xside = (GFX_MaxXGet()+1)/x;
    Yside = (GFX_MaxYGet()+1)/y;
}
