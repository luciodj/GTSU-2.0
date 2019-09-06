/*
 * File:   vs1053.c
 * 
 */
#include "vs1053.h"
#include "system.h"
#include "driver/spi/drv_spi.h"

#define USE_AND_OR

#include <ports.h>
#include "system_config.h"


// basic SCI commands
#define CODEC_CMD_WRITE       0x02
#define CODEC_CMD_READ        0x03

// basic registers list
#define CODEC_REG_MODE        0x00    // mode options
#define CODEC_REG_STATUS      0x01    // main status reg
#define CODEC_REG_BASS        0x02    // enable bass & treble enhancements
#define CODEC_REG_CLOCKF      0x03    // clock + freq. multiplier
#define CODEC_REG_DECODE_TIME 0x04    // in seconds
#define CODEC_REG_AUDATA      0x05    // misc. audio data
#define CODEC_REG_WRAM        0x06    // RAM write/read
#define CODEC_REG_WRAMADDR    0x07    // base address for RAM write/read
#define CODEC_REG_HDAT0       0x08    // stream header data 0
#define CODEC_REG_HDAT1       0x09    // stream header data 1
#define CODEC_REG_AIADDR      0x0A    // start address of application
#define CODEC_REG_VOL         0x0B    // volume control

// MODE register options
#define CODEC_MODE_DIFF      0x01     // differential (left channel inverted)
#define CODEC_MODE_LAYER12   0x02     // allow MPEG layers 1 & 2
#define CODEC_MODE_RESET     0x04     // soft reset
#define CODEC_MODE_CANCEL    0x08     // cancel decoding
#define CODEC_MODE_EARLOW    0x10     //
#define CODEC_MODE_TEST      0x20     // allow test sequences
#define CODEC_MODE_STREAM    0x40     // stream mode
#define CODEC_MODE_EARHIGH   0x80
#define CODEC_MODE_DACT      0x100    // DCLK active edge (rising, default: falling)
#define CODEC_MODE_SDIORD    0x200    // SDI bit order (default: MSb first)
#define CODEC_MODE_SDISHARE  0x400    // share SPI chip select
#define CODEC_MODE_SDINEW    0x800    // new VS1002 native mode



// prototypes
//uint16_t CODEC_Write( uint8_t b);
void CODEC_RegisterSet(uint8_t reg, uint16_t w);
uint16_t CODEC_RegisterGet( uint8_t reg);

#define CODEC_Write( x)     DRV_SPI_Put( CODEC_SPI_CHN, x)
#define CODEC_Read()        DRV_SPI_Get( CODEC_SPI_CHN)

void CODEC_WriteRegister( uint8_t reg, uint16_t w)
{
    CODEC_DCS_Disable();              // disable data bus
    CODEC_CS_Enable();                // enable command bus
    CODEC_Write( CODEC_CMD_WRITE);
    CODEC_Write( reg);
    CODEC_Write( w>>8);                //  MSB first
    CODEC_Write( w & 0xff);
    CODEC_CS_Disable();
    while( !CODEC_DREQ);
}


uint16_t CODEC_RegisterRead( uint8_t reg)
{  
    uint16_t w;
    
    CODEC_DCS_Disable();            // disable data bus
    CODEC_CS_Enable();              // enable command bus
    CODEC_Write( CODEC_CMD_READ);
    CODEC_Write( reg);
    w = CODEC_Read() ;              //  write dummy, get MSB first
    w = (w<<8) | CODEC_Read();      //  write dummy, get LSB
    CODEC_CS_Disable();
    while( !CODEC_DREQ);
    return w;
}


void CODEC_Init( uint16_t mode)
{
    DRV_SPI_INIT_DATA  slowSPI = { CODEC_SPI_CHN, slowParameters};
    DRV_SPI_INIT_DATA  fastSPI = { CODEC_SPI_CHN, fastParameters};

    // (re-)init tick timer
    SYSTEM_TickInitialize( 1);

    // Note: PPS must be already initialized to connect the SPI port to pins
    CODEC_RST_Enable();             // force reset
    CODEC_CS_Disable();
    CODEC_DCS_Disable();

    CODEC_RST_Config();             // make Reset output
    CODEC_CS_Config();              // make xCs output
    CODEC_DCS_Config();             // make xDCs output
    DRV_SPI_Initialize( &slowSPI);
    SYSTEM_DelayMs(  10);             // give time to reset
    CODEC_RST_Disable();            // release reset

    // verify that codec is busy resetting
    while ( CODEC_DREQ);

    //verify that codec is ready to receive first command
    while( !CODEC_DREQ);

    // set native + mode
    CODEC_WriteRegister( CODEC_REG_MODE, CODEC_MODE_SDINEW | mode);
    // double the codec clock frequency
    CODEC_WriteRegister( CODEC_REG_CLOCKF, 0x2000);

    if ( mode != CODEC_MODE_TEST)
    {
        // can change to full speed now
        DRV_SPI_Initialize( &fastSPI);
    }
} // MP3Init


void CODEC_SineTest( uint8_t n)
{
    // Send a Sine Test Header to Data port
    CODEC_DCS_Enable();         // enable data interface
    CODEC_Write( 0x53);         // special Sine Test Sequence
    CODEC_Write( 0xef);
    CODEC_Write( 0x6e);
    CODEC_Write( n);            // n, Fsin = Fsamp[n>>5] * (n & 0x1f) / 128
    CODEC_Write( 0x00);         // where Fsamp[x] = {44100,48000,32000,22050,
    CODEC_Write( 0x00);         //                   24000,16000,11025,12000}
    CODEC_Write( 0x00);         // for example n = 0x44 -> 1KHz
    CODEC_Write( 0x00);
    CODEC_DCS_Disable();

}

void CODEC_TestExit( void)
{
    // Stop the sine test
    CODEC_DCS_Enable();         // enable data interface
    CODEC_Write( 0x45);         // special Sine Test termination sequence
    CODEC_Write( 0x78);
    CODEC_Write( 0x69);
    CODEC_Write( 0x74);
    CODEC_Write( 0x00);
    CODEC_Write( 0x00);
    CODEC_Write( 0x00);
    CODEC_Write( 0x00);
    CODEC_DCS_Disable();

} // SineTest


void CODEC_VolumeSet( uint16_t left, uint16_t right)
{
    CODEC_WriteRegister( CODEC_REG_VOL, (left<<8) + right);

} // SetMP3Volume


void CODEC_Flush( void)
{
    int i;

    CODEC_DCS_Enable();             // select the data interface
    for( i = 0; i < 2048; i++ )     // send 2048 dummy uint8_ts to flush the FIFO
    {
        while( !CODEC_DREQ );
        CODEC_Write( 0x00 );
    }
    CODEC_DCS_Disable();
} // flush MP3


void CODEC_Feed( uint8_t **pdata, size_t *plength)
{
    int i;

    CODEC_DCS_Enable();             // select the data interface
    for( i = 0; i < 32; i++ )
    {
        CODEC_Write( *(*pdata)++);
        (*plength)--;               // decrement counter
        if( *plength == 0)          // if sent all data exit
            break;
    }
    CODEC_DCS_Disable();
} // feed MP3



