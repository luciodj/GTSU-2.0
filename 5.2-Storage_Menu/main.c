/*
 * Project: Storage_Menu
 * File:    main.c
 * 
 * Requires: MLA 1312
 */

#include "system.h"

#include "tty.h"
#include "grid.h"
#include "micro_sd.h"
#include "menu.h"

char data[ 400];
#define N_FILES     16


int main( void )
{
    SD_FILE *file;
    unsigned length;
    char *p, filename[32];


    // 4. initializations
    SYSTEM_BoardInitialize();   // init pins and ports
    Grid_Init( GFX_MaxXGet()/3, GFX_MaxYGet()/3); // defines a 3x3 grid
    TTY_Init();                 // inits terminal emulation
    DisplayBacklightOn();


    // 5. splash screen
    TTY_Clear();
    TTY_StringCenter( -1, "Menu demo");
    TTY_StringCenter( +1,  "tap to start");
    Grid_ButtonGet();
    TTY_Clear();

    SD_Initialize();

    // 3.1 ensure the file system is initialized, card inserted
    while( SD_Initialize())
    {
        TTY_StringCenter( 0, "Insert Card");
        SYSTEM_DelayMs( 100);
    }

    // 6. Main Loop
    while( 1 )
    {
        TTY_Clear();

        // 6.1 let the user choose a suitanle text file
        Menu_FileSelect( filename, "*.TXT");
        
        // 6.2 try to open a file
        if ( (file = SD_Open( filename, "r"))== NULL)
        {
            puts( "\n File Not Found");
        }

        else  // 6.3 file found, print the first buffer full
        {
            TTY_Clear();

            // grab a first batch of data
            length = SD_Read( data, 1, sizeof(data), file);

            // output to the terminal
            p = &data[0];
            while( length-- > 0 )
            {
                TTY_CharPut( *p++);
            }

            // print "..." if the file continues and close it
            if ( !SD_Eof( file))
                puts( "...");

            // 
            SD_Close( file);

        }   // else

        // 7. prompt to continue
        puts( "\n Tap to continue");
        Grid_ButtonGet();

    }   // main loop
}   // main


