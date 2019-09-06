/*
 * File:   menu.c
 *
 * Requires: MLA 1312, tty.c, grid.c
 */

#include "menu.h"
#include <string.h>

#include "gfx/gfx.h"

#define AW      40      // width
#define AH      10      // height

int Menu_SimpleMenu( char list[][16], int items)
{
    int i, n;
    code_t c;
    uint16_t midx, midy, right, bot;

    // init cursor position, first item selected
    n = 0;

    // display item list and wait for user input
    while ( 1)
    {
        // draw menu list of items
        for(i=0; i< items; i++)
        {
            // position each item on the menu centered horizontally and vertically
            TTY_CursorSet( (_MAX_X-14)/2, ( _MAX_Y - items)/2 + i);

            // temporary color scheme
            if (i != n)
            {   // non selected items are displayed in Yellow over Blue
                TTY_BackgroundSet( BLUE);
                TTY_ColorSet( YELLOW);
            }
            else
            {   // the current selected item is displayed in blue over yellow
                TTY_BackgroundSet( YELLOW);
                TTY_ColorSet( BLUE);
            }

            // each item must be padded to clear the entire line
//            TTY_Put(' ');
            printf( " %14s ", list[ i]);
//            for(j=0;j<(14-strlen( list[i])); j++)
//                TTY_Put(' ');
        } // for

        // draw arrows
        GFX_ColorSet( BLUE);
        right = GFX_MaxXGet(); midx = right>>1;
        bot   = GFX_MaxYGet(); midy = bot>>1;

        // top
        GFX_LineDraw( midx-AW, 2*AH, midx, AH);
        GFX_LineDraw( midx, AH, midx+AW, 2*AH);

        // bottom
        GFX_LineDraw( midx-AW, bot-2*AH, midx, bot-AH);
        GFX_LineDraw( midx, bot-AH, midx+AW, bot-2*AH);

        // right
        GFX_LineDraw( right-2*AH, midy-AW, right-AH, midy);
        GFX_LineDraw( right-AH, midy, right-2*AH, midy+AW);

        // restore default colors
        TTY_BackgroundSet( TTY_BACK);
        TTY_ColorSet( TTY_FORE);

        // wait for a touch input event
        c = Grid_ButtonGet();

        if ( c.x == 2)          // right side of the screen -> select
        {
            return n+1;
        }

        else if ( c.y == 0)     // top of the screen -> move cursor up
        {
            if (n > 0)  n--;
        }

        else if ( c.y == 2)     // bottom of the screen -> move cursor down
        {
            if ( n < items-1) n++;
        }

#ifdef _SCREENCAPTURE
        else if ( c.x == 0)     // left of screen capture
        {
            ScreenCapture( "screen.scr");
        }
#endif
    } // while

} // Menu


void Menu_FileSelect( char *fn, char *ext)
{ // fn    returns pointer to selected filename
  // ext   "*.AVI" selection criteria

    char list[ N_ITEMS][16];
    int n = 0;
    FILEIO_SEARCH_RECORD sr;

    while( 1)
    {
        TTY_Clear();
        
        // search for ".ext" files and put them in list
        if ( !FILEIO_Find( ext, FILEIO_ATTRIBUTE_ARCHIVE, &sr, true))
        {
            do{ // while there are files matching
                // copy the file name
                strncpy( list[ n++], (char*)(&sr.shortFileName[0]), 16);
                // check if list full
                if ( n == N_ITEMS)
                    break;
            } while ( !FILEIO_Find( ext, FILEIO_ATTRIBUTE_ARCHIVE, &sr, false));
        }

        //n = listTYPE( list, N_FILES, ext);
        if ( n > 0)
        {
            // found at least one file
            n = Menu_SimpleMenu( list, n);
        }

        // if no file found or none selected
        if ( n == 0)
        {
            //report error and allow to swap card
            TTY_StringCenter( 0, "File Not Found");
            Grid_ButtonGet();
            TTY_Clear();
        }

        else
        {
             //form the chosen filename
             strncpy( fn, list[(n-1)], 16);
             return;
        }
    }// while

} // select file
