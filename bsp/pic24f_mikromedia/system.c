/*******************************************************************************
  PIC24F Mikromedia - System Specific Initializations

// ****************************************************************************/
// Section: Includes
// *****************************************************************************
#include <xc.h>
#include <PPS.h>
#include "system.h"
#include "driver/gfx/drv_gfx_display.h"
#include "fileio/fileio.h"
#include "driver/touch_screen/drv_touch_screen.h"
#include "driver/fileio/sd_spi.h"
#include "driver/nvm/drv_nvm_flash_spi_m25p80.h"

// *****************************************************************************
// Configuration bits
// *****************************************************************************

// PIC24FJ256GB110 Configuration Bit Settings

// CONFIG3
#pragma config WPFP = WPFP511           // Write Protection Flash Page Segment Boundary (Highest Page (same as page 170))
#pragma config WPDIS = WPDIS            // Segment Write Protection Disable bit (Segmented code protection disabled)
#pragma config WPCFG = WPCFGDIS         // Configuration Word Code Page Protection Select bit (Last page(at the top of program memory) and Flash configuration words are not protected)
#pragma config WPEND = WPENDMEM         // Segment Write Protection End Page Select bit (Write Protect from WPFP to the last page of memory)

// CONFIG2
#pragma config POSCMOD = XT             // Primary Oscillator Select (XT oscillator mode selected)
#pragma config DISUVREG = ON            // Internal USB 3.3V Regulator Disable bit (Regulator is enabled)
#pragma config IOL1WAY = OFF            // IOLOCK One-Way Set Enable bit (Unlimited Writes To RP Registers)
#pragma config OSCIOFNC = OFF           // Primary Oscillator Output Function (OSCO functions as CLKO (FOSC/2))
#pragma config FCKSM = CSDCMD           // Clock Switching and Monitor (Both Clock Switching and Fail-safe Clock Monitor are disabled)
#pragma config FNOSC = PRIPLL           // Oscillator Select (Primary oscillator (XT, HS, EC) with PLL module (XTPLL,HSPLL, ECPLL))
#pragma config PLL_96MHZ = ON           // 96MHz PLL Disable (Enabled)
#pragma config PLLDIV = DIV2            // USB 96 MHz PLL Prescaler Select bits (Oscillator input divided by 2 (8MHz input))
#pragma config IESO = OFF               // Internal External Switch Over Mode (IESO mode (Two-speed start-up)disabled)

// CONFIG1
#pragma config WDTPS = PS32768          // Watchdog Timer Postscaler (1:32,768)
#pragma config FWPSA = PR128            // WDT Prescaler (Prescaler ratio of 1:128)
#pragma config WINDIS = OFF             // Watchdog Timer Window (Standard Watchdog Timer enabled,(Windowed-mode is disabled))
#pragma config FWDTEN = OFF             // Watchdog Timer Enable (Watchdog Timer is disabled)
#pragma config ICS = PGx2               // Comm Channel Select (Emulator functions are shared with PGEC2/PGED2)
#pragma config GWRP = OFF               // General Code Segment Write Protect (Writes to program memory are allowed)
#pragma config GCP = OFF                // General Code Segment Code Protect (Code protection is disabled)
#pragma config JTAGEN = OFF             // JTAG Port Enable (JTAG port is disabled)

#ifdef _SFLASH
    const DRV_SPI_INIT_DATA _nvmP = { SFLASH_SPI_CHN, fastParameters};
#endif

// *****************************************************************************
// void SYSTEM_BoardInitialize(void)
// *****************************************************************************
void SYSTEM_BoardInitialize(void)
{

    // ---------------------------------------------------------
    // Make sure the display DO NOT flicker at start up
    // ---------------------------------------------------------
    DisplayBacklightConfig();
    DisplayPowerConfig();
    DisplayBacklightOff();

    // ---------------------------------------------------------
    // Initialize the Display Driver
    // ---------------------------------------------------------
    DRV_GFX_Initialize();

    // make all pin digital by default
    AD1PCFG = 0xFFFF;   // all inputs (not touch) digital

    // configure PPS for PIC24 Mikromedia
    PPSUnLock;

    // SPI2
    PPSInput( PPS_SDI2,  PPS_RP26);     // SDI2 =RP26 G7/pin 11
    PPSOutput( PPS_RP21, PPS_SCK2OUT);  // SCK2 =RP21 G6/pin 10
    PPSOutput( PPS_RP19, PPS_SDO2);     // SDO2 =RP19 G8/pin 12

    // configure Serial Flash CS pin
    M25P80_CS_LAT = 1;
    M25P80_CS_TRIS = 0;

#ifdef _SFLASH
    // configure SPI for Serial Flash
    DRV_NVM_M25P80_Initialize( (void*)&_nvmP);
#endif

    // configure Codec CS pin
    CODEC_CS_Disable();
    CODEC_CS_Config();
    CODEC_DCS_Disable();
    CODEC_DCS_Config();

    SYSTEM_TickInitialize( 1);
#ifdef _SFLASH
    TouchInit( NVMWrite, NVMRead, NVMSectorErase, NULL);
#else
//    TouchInit( NULL, NULL, NULL, NULL);
#endif

    GFX_Initialize();

} // Board Initialize


/**
 * RTCC_Init()
 *      initializes the RTCC module for 1/2 periodic alarm
 */
void SYSTEM_RTCCInitialize( void)
{
    RtccInitClock();            // init 32KHz oscillator

    __builtin_write_RTCWEN();   // unlock RTCC registers
    mRtccOn();                  // enables rtcc, also clears alarm cfg
    ALCFGRPT = 0xc000;          // enable alarm, chime, 1/2 second period
}


/**
 * Tick_Init()
 *    Initializes the tick periodic timer )
 * @param period_ms
 */
void SYSTEM_TickInitialize( unsigned period_ms)
{
    // Initialize Timer3
    TMR3 = 0;
    PR3 = TICK_PERIOD( period_ms);
    T3CONbits.TCKPS = 1;        // Set prescale to 1:8
    IFS0bits.T3IF = 0;          // Clear flag
    IEC0bits.T3IE = 1;          // Enable interrupt
    T3CONbits.TON = 1;          // Run timer
}

volatile unsigned TickCounter = 0;

// stubs for interrupt services
int16_t __attribute__((weak)) TouchDetectPosition( void) { return -1;};
void __attribute__((weak)) CodecCallBack( void){};

void _ISR_T3 _T3Interrupt( void)
{
    _T3IF = 0;
    TickCounter++;
    TouchDetectPosition();
    CodecCallBack();
}


unsigned SYSTEM_TickCounterGet( void)
{
    return TickCounter;
}


void SYSTEM_DelayMs( unsigned ms)
{
    unsigned t = SYSTEM_TickCounterGet() + ms;
    while( t > SYSTEM_TickCounterGet())
        Nop();
    Nop();
    Nop();
}


void SYSTEM_BacklightInitialize()
{
    // configure OC1 block to generate a PWM signal
    OC1CON1bits.OCTSEL = 0x7;           // use peripheral clock (16Mhz)
    OC1CON1bits.OCM = 0x6;              // edge aligned PWM
    OC1CON2 = 0;                        // No SYnchronizations required
    OC1R =  0xfffe;                     // start off with max value
    OC1RS = 0xffff;                     // set period  ~ 240Hz (16MHz/65.536)
    PPSOutput( PPS_RP23, PPS_OC1);      // OC1 =RP23 D2/pin 77
} // Backlight Init


void SYSTEM_BacklightSet( int i)
{
    OC1R = i<<8;
}

