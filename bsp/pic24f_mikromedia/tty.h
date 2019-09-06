/******************************************************************************
 *
 *   Terminal Emulation on MikroE Mikromedia TFT display
 *
 *******************************************************************************/

#include "tty_config.h"
#include "system_config.h"
#include "gfx_config.h"
#include "gfx/gfx.h"


void TTY_Init(void);
void TTY_Home(void);
void TTY_Clear(void);
void TTY_Put( char);
void TTY_CharPut( char);
void TTY_StringPut( char *);
void TTY_StringCenter( int, char *);
void TTY_CursorLeft(void);
void TTY_CursorRight(void);
void TTY_CursorUp(void);
void TTY_CursorDown(void);
void TTY_LineHome(void);
void TTY_ClearToEOL( void);
void TTY_CursorSet( int, int);
void TTY_ColorSet( int x);
void TTY_BackgroundSet( int x);
int TTY_ColorGet( void);
int TTY_BackgroundGet( void);

// font size is based on default font 
#define FONT_W  10
#define FONT_H  15

#define _MAX_X  (GFX_MaxXGet()+1)/FONT_W
#define _MAX_Y  (GFX_MaxYGet()+1)/FONT_H

#define AT( x, y)   TTY_CursorSet( x, y)

