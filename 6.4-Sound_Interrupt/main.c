/*
 * Project: Sound_Interrupt
 * File:    main.c
 *
 * Requires: MLA 1312
 */

#include "system.h"
#include "vs1053.h"
#include "tty.h"
#include "micro_sd.h"


int fPLAY = false;
SD_FILE  *file;

void CodecCallBack( void)
{
    static size_t length = 0;
    static uint8_t buffer[512], *p;

    if ( !fPLAY)
        return;

    while( 1)
    {
        // ensure buffer is loaded
        if ( length == 0)            // if buffer empty
        {
            length = SD_Read( buffer, 1, sizeof( buffer), file);
            p = buffer;

            if ( length == 0)       // nothing else to read (EOF)
            { // rewind for next use
                CODEC_Flush();
                fPLAY = false;
                return;
            }
        }

        // check if MP3 needs feeding
        if ( CODEC_DREQ)
        {
            // feed the hungry codec
            CODEC_Feed( &p, &length);
        }
        else return;
    }
} // background playback 


int main( void )
{    
    // 1. initializations
    SYSTEM_BoardInitialize();
    TTY_Init();
    TTY_Clear();
    DisplayBacklightOn();

    // init the MP3 Decoder
    CODEC_Init( 0);
    CODEC_VolumeSet( 30, 30);
    Tick_DelayMs(1);

    FILEIO_Initialize();

    // init file system, wait for SD card to be inserted
    while( SD_Initialize())
    {
        TTY_StringCenter( 0, "Insert Card");
        Tick_DelayMs( 200);
    }

    // signal card detected and mounted
    TTY_BackgroundSet( GREEN);
    TTY_Clear();              // show green screen if successful initializing

    // try to open an MP3 file
    if ( (file = SD_Open( "SONG.MP3", "r"))==NULL)
    {
        TTY_BackgroundSet( BRIGHTRED);
        TTY_Clear();
        while(1);
    }

    fPLAY = true;       // have the playback start
    
    // 2. Main Loop
    while( 1 )
    {

    } // main loop
}


