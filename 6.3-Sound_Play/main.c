/*
 * Project: Sound_Play
 * File:    main.c
 *
 * Requires: MLA 1312
 */

#include "system.h"
//#include "grid.h"
#include "tty.h"
#include "vs1053.h"
#include "micro_sd.h"
#include "driver/fileio/sd_spi.h"


int main( void )
{
    SD_FILE * file;
    BYTE data[ 512];
    size_t length = 0;
    BYTE *p;
    
    // 1. initializations
    SYSTEM_BoardInitialize();
//    Grid_Init( GFX_MaxXGet()/3, GFX_MaxYGet()/3);
    TTY_Init();
    TTY_Clear();
    DisplayBacklightOn();

//    Tick_Init(1);
    // init the MP3 Decoder
    CODEC_Init( 0);
    CODEC_VolumeSet( 30, 30);
    Tick_DelayMs(2);

    FILEIO_Initialize();

    // init file system, wait for SD card to be inserted
    while( SD_Initialize() )
    {
        TTY_StringCenter( 0, "Insert Card");
        Tick_DelayMs(100);
    }

    // signal card detected and mounted
    TTY_BackgroundSet( GREEN);
    TTY_Clear();              // show green screen if successful initializing

    // try to open an MP3 file
    if ( (file = SD_Open( "SONG.MP3", "r")) == NULL)
    {
        TTY_BackgroundSet( BRIGHTRED);
        TTY_Clear();          // show red screen if could not find the file
        while(1);
    }

    // 2. Main Loop
    while( 1 )
    {
        // 3. check if buffer ready
        if (length == 0)
        {
            CODEC_DCS_Disable();

            // 4. fetch more data
            length = SD_Read( data, 1, sizeof(data), file);
            p = data;

            if (length==0)                  // 6. eof
            {
                CODEC_Flush();              // flush buffer
                while(1);
            }
        }

        // wait if codec not ready to get more data
        while ( !CODEC_DREQ)
        {
             // add your task here
        }

        // 5. feed the codec
        CODEC_Feed( &p, &length);

    } // main loop
}
