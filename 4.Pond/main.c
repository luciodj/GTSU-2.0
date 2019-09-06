/*
 * Project: Pond
 *
 * Simple animation using graphics primitives and touchscreen input
 */
#include <xc.h>
#include "system.h"
#include "driver/touch_screen/drv_touch_screen.h"
#include "tty.h"

#define RADIUS      20
#define SPEED       20
#define MAX_LIST    20

typedef struct {            // drop coordinates and radius
    unsigned x;
    unsigned y;
    unsigned r;
} drop_t;

drop_t drop[ MAX_LIST];     // list containing each drop

int main( void)
{
    drop_t *p, *t;
    unsigned list_len = 0;
    unsigned temp_len = 0;
    int i;

    // initializations
    SYSTEM_BoardInitialize();
    TTY_ColorSet( LIGHTBLUE);
    TTY_BackgroundSet( BLACK);
    TTY_Init();
    DisplayBacklightOn();

    // splash screen
    TTY_StringCenter( -1, "The Pond");
    TTY_StringCenter( +1, "Ready...");
    while( TouchGetX() == -1);      // wait for touch
    while( TouchGetX() != -1);      // release

    TTY_Clear();

    // main loop
    while(1)
    {
        SYSTEM_DelayMs( 100);

        // check if a new drop has arrived
        if ( TouchGetX() != -1)
            if ( list_len < MAX_LIST)
            {
                // list_append
                drop[list_len].x = TouchGetX();
                drop[list_len].y = TouchGetY();
                drop[list_len].r = RADIUS;
                list_len++;
            }

        // update and draw each drop in the list
        temp_len = 0;
        p = &drop[0];
        t = &drop[0];
        for( i=0; i<list_len; i++)
        {
            // erase previous position
            GFX_ColorSet( BLACK);
            GFX_CircleDraw( p->x, p->y, p->r);
            
            // enlarge circle
            p->r += SPEED;

            // if a circle is technically still visible
            if ( p->r < 256)
            {
                if ( t != p)    // move the drop down the list
                {
                    t->x = p->x;
                    t->y = p->y;
                    t->r = p->r;
                }
                // draw growing circles with decreasing intensity
                GFX_ColorSet( GFX_RGBConvert( 0, 0, 256 - t->r ));
                GFX_CircleDraw( t->x, t->y, t->r);
                // grow the temp list
                t++;
                temp_len++;
            }
            p++;
        }

        // update list length
        list_len = temp_len;
    } // main loop

    return 0;
} // main