/*******************************************************************************
  PIC24F Mikromedia  - System Specific Definitions

*******************************************************************************/

#ifndef __SYSTEM_H
#define __SYSTEM_H

/*********************************************************************
* PIC Device Specific includes
*********************************************************************/
#include <xc.h>
#include <stdio.h>
#include <stdint.h>

#include "system_config.h"
#include "fonts/veramono12.h"

// when using GOL
#include "gfx/gfx.h"
#include "driver/touch_screen/drv_touch_screen.h"

/*********************************************************************
* Macro: #define	SYS_CLK_FrequencySystemGet()
*
* Overview: This macro returns the system clock frequency in Hertz.
*			* value is 8 MHz x 4 PLL for PIC24
*
********************************************************************/
#define SYS_CLK_FrequencySystemGet()    (32000000ul)

/*********************************************************************
* Macro: #define	SYS_CLK_FrequencyPeripheralGet()
*
* Overview: This macro returns the peripheral clock frequency
*			used in Hertz.
*			* value for PIC24 is <PRE>(SYS_CLK_FrequencySystemGet()/2) </PRE>
*
********************************************************************/
#define SYS_CLK_FrequencyPeripheralGet()    (SYS_CLK_FrequencySystemGet() / 2)

/*********************************************************************
* Macro: #define	SYS_CLK_FrequencyInstructionGet()
*
* Overview: This macro returns instruction clock frequency
*			used in Hertz.
*			* value for PIC24 is <PRE>(SYS_CLK_FrequencySystemGet()/2) </PRE>
*
********************************************************************/
#define SYS_CLK_FrequencyInstructionGet()   (SYS_CLK_FrequencySystemGet() / 2)

/**********************************************************************
 * __delay_ms define - FCY must be defined before including
 * libpic30.h
 **********************************************************************/
#define FCY             (SYS_CLK_FrequencyInstructionGet())

#include <libpic30.h>

/*********************************************************************
* Board initialization
*********************************************************************/
void SYSTEM_BoardInitialize(void);

typedef enum
{
    ANSEL_DIGITAL = 0,
    ANSEL_ANALOG = 1
}ANSEL_BIT_STATE;

#include <rtcc.h>

/*********************************************************************
* RTCC CONTROLLER INTERFACE
*********************************************************************/
#define __RTCC_ISR    __attribute__((interrupt, shadow, no_auto_psv))

#define RTCC_InterruptEnable()      ( _RTCIE = 1)
#define RTCC_InterruptFlagClear()   ( _RTCIF = 0)

void SYSTEM_RTCCInitialize( void);


/*********************************************************************
 * LOW POWER MODES
 *********************************************************************/
#define CORE_Sleep()    Sleep()

/*********************************************************************
 * TICK - CONTROLLER INTERFACE
 *********************************************************************/

#define _ISR_T3  __attribute__((interrupt, shadow, no_auto_psv))
#define TICK_PERIOD( ms)  (SYS_CLK_FrequencyPeripheralGet() * (ms)) / 8000
void SYSTEM_TickInitialize( unsigned ms);

void SYSTEM_DelayMs( unsigned ms);

/*********************************************************************
 * Backlight - DISPLAY Backligh INTERFACE
 *********************************************************************/
void SYSTEM_BacklightInitialize( void);
void SYSTEM_BacklightSet( int i);


#endif




