

#ifndef _SYSTEM_H
#define _SYSTEM_H

#include <xc.h>
#include <rtcc.h>

/*********************************************************************
* RTCC CONTROLLER INTERFACE
*********************************************************************/
#define __RTCC_ISR    __attribute__((interrupt, shadow, no_auto_psv))

#define RTCC_InterruptEnable()      ( _RTCIE = 1)
#define RTCC_InterruptFlagClear()   ( _RTCIF = 0)

void RTCC_Init( void);


/*********************************************************************
 * LOW POWER MODES
 *********************************************************************/
#define CORE_Sleep()    Sleep()

/*********************************************************************
 * Display Backlight
 *********************************************************************/
#define DISPLAY_BacklightSet( x)    ( PORTDbits.RD2 = x)
#define DISPLAY_BacklightGet()      PORTDbits.RD2
#define DISPLAY_BacklightConfig()   ( TRISDbits.TRISD2 = 0)
#endif