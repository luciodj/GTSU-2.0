/******************************************************************************
 *
 *   Terminal Emulation on MikroE Mikromedia TFT display
 *
 *******************************************************************************

* File Name:       LCDterminal.c

*******************************************************************************/

#include <string.h>
#include "tty.h"
#include "fonts/veramono12.h"

static int _cx, _cy;    // cursor position
static int _back = TTY_BACK , _fore = TTY_FORE;

#ifdef TTY_SCROLL
// text page char matrix
static char page[ _MAX_Y][ _MAX_X+1];
#endif


int TTY_XGet()
{ 
    return _cx;
}


int TTY_YGet()
{ 
    return _cy;
}


void TTY_ColorSet( int x)
{
    _fore = x;
}    


void TTY_BackgroundSet( int x)
{
    _back = x;
}    


int TTY_ColorGet()
{
    return _fore;
}


int TTY_BackgroundGet()
{
    return _back;
}


void TTY_Home(void)
{   // set cursor to home position
    _cx = 0; 
    _cy = 0;
}


void TTY_CursorSet( int x, int y)
{
    _cx = x; _cy = y;
}


void TTY_LineHome(void)
{   
    _cx = 0;
}    

void TTY_ClearToEOL( void)
{
    int t = _cx;
    int i;
    
    // fill with spaces until the end of the line
    for( i=_cx; i< _MAX_X; i++)
        TTY_Put( ' ');
        
    // return to position
    _cx = t;    
}

void TTY_Clear(void)
{
#ifdef TTY_SCROLL
    int i,j;
    for (i=0; i< _MAX_X; i++)
        for( j=0; j<_MAX_Y; j++)
            page[j][i] = ' ';
#endif
            
    GFX_ColorSet( _back);
    GFX_ScreenClear();
    GFX_ColorSet( _fore);
    TTY_Home();
}

void TTY_Init(void)
{
    GFX_Initialize();       // initialize graphics library
    TTY_Clear();
    GFX_FontSet( (void *) &VeraMono_12);    // set font
}


void TTY_CursorLeft(void)
{
    if (_cx>0) 
        _cx--;
}


void TTY_CursorUp(void)
{
    if ( _cy>0) _cy--;
}


void TTY_CursorDown(void)
{    
    // advance to next line
    _cy++;

#ifdef  TTY_SCROLL
    if ( _cy >= _MAX_Y) 
    {   // scroll entire screen up
        int i,j;
        for( j=0; j<_MAX_Y-1; j++)
        {   // for each line
            // clear the line background (indipendent from TTY_OVERLAY)
            GFX_ColorSet( _back);
            GFX_BarDraw( 0, FONT_H*j, GFX_MaxXGet()-1, FONT_H*(j+1));
            GFX_ColorSet( _fore);
            // copy from next line print the new line content
            for( i=0; i<_MAX_X; i++)
            {
                page[j][i] = page[j+1][i];
                GFX_TextCursorPositionSet( FONT_W*i, FONT_H*j);
                GFX_TextCharDraw( page[j][i]);
            }    
        }

        // limit to last line
        _cy = _MAX_Y-1;

        // clean up the last line
        GFX_ColorSet( _back);
        GFX_BarDraw( 0, FONT_H*_cy, GFX_MaxXGet()-1, FONT_H*(_cy+1));
        for( j=0; j<_MAX_X; j++) page[_cy][j]=' ';
        GFX_ColorSet( _fore);

    }
#else   // no scrolling option roll
    if ( _cy >= _MAX_Y) 
    {
        _cy = 0;
    }    

#endif        
}


void TTY_CursorRight(void)
{
    _cx++;
#ifdef TTY_WRAP
    if ( _cx >= _MAX_X) 
    {   // wrap to a new line
        _cx = 0;    
        TTY_CursorDown();
    }    
#endif
}


void TTY_Put(char A)
{
    // clipping
    if (( _cx>=_MAX_X) || (_cy>=_MAX_Y))   // clip
    {
        _cx = 0;
        _cy = 0;
        return;
    }

    if (( A<' ')|| ( A>127))
        return;

#ifdef TTY_SCROLL
        page[ _cy][ _cx] = A;
#endif

#ifndef TTY_OVERLAY
    // clear the background
    GFX_ColorSet( _back);
    GFX_BarDraw( _cx*FONT_W, _cy*FONT_H, (_cx+1)*FONT_W, (_cy+1)*FONT_H);
    GFX_ColorSet( _fore);
#endif

    GFX_TextCursorPositionSet( FONT_W * _cx, FONT_H*_cy);
    GFX_TextCharDraw( A);
    TTY_CursorRight();
} // TTY_Put


void TTY_CharPut(char A)
{
    int tab, i;
    
    switch( A)
    {
      case '\b':    // backstep
        if ( _cx>0)
        {
            _cx--;
            TTY_Put(' ');
            _cx--;
        }    
        break;
        
      case '\t':    // move to next tab position
        tab = (_cx/8 + 1) * 8;
        // add spaces until the next tab stop
        for( i=_cx; i<tab-1; i++)
            TTY_Put(' ');
        break;

      case '\n':    // New Line
//          printf("<ln>");
        TTY_CursorDown();
        // break;   // continue into Home
        
      case '\r':    // Home
//          printf("<cr>");
#ifndef TTY_OVERLAY        // clear rest of the line
            GFX_ColorSet( _back);
            GFX_BarDraw( FONT_W*_cx, FONT_H*_cy, GFX_MaxXGet()-1, FONT_H*(_cy+1));
            GFX_ColorSet( _fore);
#endif
        _cx = 0;
        break;

      default:      // print-able char
        TTY_Put( A);
        break;
    } // switch
    
} // TTY_PutChar


int write( int handle, char *p, unsigned len )
{
    unsigned i = len;

    while ( i-- > 0)
        TTY_CharPut( *p++);

    return len;
}


void TTY_StringPut( char* s)
{
    char c;
    while( (c = *s++))
        TTY_CharPut( c);
} //TTY_PUtString


void TTY_StringCenter( int p, char *s)
{  // p  integer offset (lines) above or below center
   // s  string
   // differs from MMB similar function by using Terminal mono space font
    int x, y;

    // get center position
    y = _MAX_Y/2 - 1 + p;
    x = (_MAX_X - strlen( s))/2;

    // set and print
    TTY_CursorSet(  x, y);
    TTY_StringPut( s);
} // TTY_CenterString

