/*
 * Project: 6.2-Sound_Hello
 *
 * File:   main.c
 */

#include "system.h"
#include "vs1053.h"

#include "sounds/hello.h"


int main( void )
{
    size_t length;
    uint8_t *p;

    // initializations
    SYSTEM_BoardInitialize();          // init PPS

    // init the MP3 player
    CODEC_Init( CODEC_NORMAL);                    // init decoder, default
    CODEC_VolumeSet( 30, 30);          // set volume
    
    // Main Loop
    while( 1 )
    {
        CODEC_SineTest( 0x44);
        
        // 1. init pointer to Flash audio resource
        p = (void*) Hello;
        length = sizeof( Hello);

        // 2. play back entire audio resource
        while ( length > 0 )
        {
            while ( !CODEC_DREQ);    
            CODEC_Feed( &p, &length);
        }

        CODEC_Flush();                 // FlushMP3 buffer

        // 3. repeat after a brief pause
        Tick_DelayMs(200);
        
    } // main loop
}



