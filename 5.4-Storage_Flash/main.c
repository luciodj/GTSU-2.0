/*
 * Project Storage_flash
 *
 * File:  main.c
 *
 * Requires: MLA 1312
 */

#include "system.h"
#include "tty.h"
#include "grid.h"
#include "menu.h"
#include "micro_sd.h"
#include "driver/nvm/drv_nvm_flash_spi_m25p80.h"


uint8_t data[ 4096];           // ensure that ERASE_SECTOR_SIZE is divisible by sizeof(data)!

int main( void )
{
    SD_FILE *file;
    unsigned length;
    char filename[32];
    uint32_t address, i, scount;
    uint8_t a;

    // 1. initializations
    SYSTEM_BoardInitialize();                   // init pins and ports
//    DRV_NVM_M25P80_Initialize( (void*)&nvmParameters);
    Grid_Init( 3, 3);                           // defines a 3x3 grid
    TTY_Init();                                 // inits terminal emulation
    DisplayBacklightOn();

    // 2. splash screen
    TTY_StringCenter( -1, "Serial Flash demo");
    TTY_StringCenter( +1, "Tap to start");
    Grid_ButtonGet();


    while( SD_Initialize())
    {
        TTY_StringCenter( 0, "Insert Card");
        SYSTEM_DelayMs(100);
    }

    // 3. Main Loop
    while( 1 )
    {
        TTY_Clear();

        // 4. try to open a file
        Menu_FileSelect( filename, "*.TXT");
        TTY_Clear();

        // 5. open file
        if ( (file = SD_Open( filename, "r")) == NULL)
        {
            TTY_StringCenter( 0, "Cannot open file");
        }

        else // file found
        {
            address = 0;        // absolute address
            scount = 0;         // sector relative count
            TTY_Clear();

            // 6. copy file to serial flash
            while ( !SD_Eof( file))
            {
                // 7. erase sector at the beginning
                if ( scount == 0)
                {
                    DRV_NVM_M25P80_SectorErase( address);
                    puts( "***Erasing Sector***");
                }

                // 8. copy (up to) one sector of data at a time
                length = SD_Read( &data, 1, sizeof( data), file);
                puts( "***Writing Data***");

                // 9. write to device
                DRV_NVM_M25P80_Write( address, data, length);
                address += length;
                
                // 10. roll over the counter within a sector
                scount  += length;
                if ( scount > ERASE_SECTOR_SIZE)
                    scount = 0;
            }

            SD_Close( file);

            // 11. read back and display on terminal
            puts( "***Enf of File***");
            puts( "Tap to read back");
            Grid_ButtonGet();

            for ( i=0; i<address; i++)
            {   // read back one char at a time
                DRV_NVM_M25P80_Read( i, &a, 1); 
                putchar( a);
            }
            puts("");
            puts( "Enf of File");

        } // copy

        // 12. prompt to continue
        puts("\nTap to continue...");
        Grid_ButtonGet();

    }   // main loop
}   // main


