/*
 * Project: 5.1-Storage_Text
 * File:    main.c
 *
 * Desc:    Basic example of use of the FILEIO library
 *
 * Requires: MLA 1312
 */

#include "system.h"

#include "tty.h"
#include "grid.h"
#include "micro_sd.h"

char data[ 400];        // a buffer of arbitrary length


int main( void )
{
    SD_FILE *fp;
    unsigned length;
    char *p;

    // 2. initializations
    SYSTEM_BoardInitialize();           // init pins and ports
    Grid_Init( 3, 3);                   // defines a 3x3 grid
    TTY_Init();                         // inits terminal emulation
    DisplayBacklightOn();

    // 3. splash screen
    TTY_Clear();
    TTY_StringCenter( -1, "FILEIO demo");
    TTY_StringCenter( +1,  "tap to start");
    Grid_ButtonGet();
    TTY_Clear();
    
    // Main Loop
    while( 1 )
    {
        TTY_Clear();

        // 4. init file system, wait for SD card to be inserted
        while( SD_Initialize())
        {
            TTY_StringCenter( 0, "Insert SD Card");
            SYSTEM_DelayMs( 100);
        }

        // 5. try to open a file
        if ( (fp = SD_Open( "README.TXT", "r")) == NULL)
        {
            puts( "File Not Found");
        }

        else    // file found
        {
            // 6. output the content of the first buffer read
            length = FILEIO_Read( data, 1, sizeof(data), fp);
            p = &data[0];
            while( length-- > 0 )
            {
                TTY_CharPut( *p++);
            }

            // 7. print "..." if the file continues
            if ( !FILEIO_Eof( fp))
                puts( "...");

            FILEIO_Close( fp);
        }   // else

        // 8. prompt to continue
        puts( "Tap to continue");
        Grid_ButtonGet();

    }   // main loop
}   // main


