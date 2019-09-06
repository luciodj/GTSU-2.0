#include "system.h"

_CONFIG1( JTAGEN_OFF        // disable JTAG interface
        & GCP_OFF           // disable general code protection
        & GWRP_OFF          // disable flash write protection
        & ICS_PGx2          // ICSP interface (2=default)
        & FWDTEN_OFF)       // disable watchdog timer

// no need to enable the external 8MHz crystal osc and PLL
// to save power, use the internal oscillator (default) and rely on the RTCC
// for accurate timing

//_CONFIG2( PLL_96MHZ_ON      // enable USB PLL module
//        & PLLDIV_DIV2       // 8MHz/2 = 4Mhz input to USB PLL
//        & IESO_OFF          // two speed start up disabled
//        & FCKSM_CSDCMD      // disable clock-switching/monitor
//        & FNOSC_PRIPLL      // primary oscillator: enable PLL
//        & POSCMOD_XT)       // primary oscillator: XT mode

/**************/



/**
 * RTCC_Init()
 *      initializes the RTCC module for 1/2 periodic alarm
 */
void RTCC_Init( void)
{
    RtccInitClock();            // init 32KHz oscillator

    __builtin_write_RTCWEN();   // unlock RTCC registers
    mRtccOn();                  // enables rtcc, also clears alarm cfg
    ALCFGRPT = 0xc000;          // enable alarm, chime, 1/2 second period
}

