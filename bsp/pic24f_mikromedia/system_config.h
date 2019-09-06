/*******************************************************************************
  System Specific Definitions

  Company:
    Microchip Technology Inc.

  File Name:
    system_config.h

  Summary:
    System level definitions for the specific Microchip Development Board used.

*******************************************************************************/

#ifndef __SYSTEM_CONFIG_H
#define __SYSTEM_CONFIG_H

/*********************************************************************
* Project Specific includes
*********************************************************************/
#include "gfx_config.h"
#include "fileio_config.h"
#include "sd_spi_config.h"
#include "drv_spi_config.h"

//#define _SFLASH

#define USE_8BIT_PMP
#define GFX_USE_DISPLAY_CONTROLLER_ILI9341

/*********************************************************************
* DISPLAY PARAMETERS
*********************************************************************/


#define DISP_ORIENTATION    90
#define DISP_HOR_RESOLUTION 240
#define DISP_VER_RESOLUTION 320

// define the SPI channel used
#define DRV_SPI_CONFIG_CHANNEL_2_ENABLE

/*********************************************************************
* DISPLAY PARALLEL INTERFACE
*********************************************************************/
#define USE_GFX_PMP

/*********************************************************************
* PARALLEL MASTER PORT INTERFACE TIMING
*********************************************************************/
// based on the ILI9341 data sheet
#define PMP_DATA_SETUP_TIME       (0)
#define PMP_DATA_WAIT_TIME        (0)
#define PMP_DATA_HOLD_TIME        (1)

/*********************************************************************
* HARDWARE PROFILE FOR DISPLAY CONTROLLER INTERFACE
*********************************************************************/
#define BACKLIGHT_ENABLE_LEVEL      1
#define BACKLIGHT_DISABLE_LEVEL     0

// Definitions for reset pin
#define DisplayResetConfig()        TRISCbits.TRISC1 = 0
#define DisplayResetEnable()        LATCbits.LATC1 = 0
#define DisplayResetDisable()       LATCbits.LATC1 = 1

// Definitions for RS pin
#define DisplayCmdDataConfig()      TRISBbits.TRISB15 = 0
#define DisplaySetCommand()         LATBbits.LATB15 = 0
#define DisplaySetData()            LATBbits.LATB15 = 1

// Definitions for CS pin
#define DisplayConfig()             TRISFbits.TRISF12 = 0
#define DisplayEnable()             LATFbits.LATF12 = 0
#define DisplayDisable()            LATFbits.LATF12 = 1

// Definitions for FLASH CS pin
#define DisplayFlashConfig()
#define DisplayFlashEnable()
#define DisplayFlashDisable()

// Definitions for POWER ON pin
#define DisplayPowerConfig()
#define DisplayPowerOn()
#define DisplayPowerOff()

// Definitions for backlight control pin
#define DisplayBacklightConfig()    ( TRISDbits.TRISD2 = 0)
#define DisplayBacklightOn()        ( LATDbits.LATD2 = BACKLIGHT_ENABLE_LEVEL)
#define DisplayBacklightOff()       ( LATDbits.LATD2 = BACKLIGHT_DISABLE_LEVEL)


/*********************************************************************
* IOS FOR THE RESISTIVE TOUCHSCREEN
*********************************************************************/

// definitions for resistive touch screen
// ADC Input Channel Selector
#define TOUCH_ADC_INPUT_SEL     AD1CHS
// ADC Sample Start
#define TOUCH_ADC_START         AD1CON1bits.SAMP
// ADC Status
#define TOUCH_ADC_DONE          AD1CON1bits.DONE

// resistive touch screen analog pins
#define ADC_XPOS                13
#define ADC_YPOS                12

#define ADPCFG_XPOS             AD1PCFGbits.PCFG13
#define ADPCFG_YPOS             AD1PCFGbits.PCFG12

#define RESISTIVETOUCH_ANALOG   0
#define RESISTIVETOUCH_DIGITAL  1

// X port definitions
#define ResistiveTouchScreen_XPlus_Drive_High()         LATBbits.LATB13   = 1
#define ResistiveTouchScreen_XPlus_Drive_Low()          LATBbits.LATB13   = 0
#define ResistiveTouchScreen_XPlus_Config_As_Input()    TRISBbits.TRISB13 = 1
#define ResistiveTouchScreen_XPlus_Config_As_Output()   TRISBbits.TRISB13 = 0

#define ResistiveTouchScreen_XMinus_Drive_High()        LATBbits.LATB11   = 1
#define ResistiveTouchScreen_XMinus_Drive_Low()         LATBbits.LATB11   = 0
#define ResistiveTouchScreen_XMinus_Config_As_Input()   TRISBbits.TRISB11 = 1
#define ResistiveTouchScreen_XMinus_Config_As_Output()  TRISBbits.TRISB11 = 0

    // Y port definitions
#define ResistiveTouchScreen_YPlus_Drive_High()         LATBbits.LATB12   = 1
#define ResistiveTouchScreen_YPlus_Drive_Low()          LATBbits.LATB12   = 0
#define ResistiveTouchScreen_YPlus_Config_As_Input()    TRISBbits.TRISB12 = 1
#define ResistiveTouchScreen_YPlus_Config_As_Output()   TRISBbits.TRISB12 = 0

#define ResistiveTouchScreen_YMinus_Drive_High()        LATBbits.LATB10   = 1
#define ResistiveTouchScreen_YMinus_Drive_Low()         LATBbits.LATB10   = 0
#define ResistiveTouchScreen_YMinus_Config_As_Input()   TRISBbits.TRISB10 = 1
#define ResistiveTouchScreen_YMinus_Config_As_Output()  TRISBbits.TRISB10 = 0

// Touch Screen Non-Volatile Memory Storage Macros
#define ADDRESS_RESISTIVE_TOUCH_VERSION	(unsigned long)0xFFFFFFFE
#define ADDRESS_RESISTIVE_TOUCH_ULX     (unsigned long)0xFFFFFFFC
#define ADDRESS_RESISTIVE_TOUCH_ULY     (unsigned long)0xFFFFFFFA
#define ADDRESS_RESISTIVE_TOUCH_URX     (unsigned long)0xFFFFFFF8
#define ADDRESS_RESISTIVE_TOUCH_URY     (unsigned long)0xFFFFFFF6

#define ADDRESS_RESISTIVE_TOUCH_LLX     (unsigned long)0xFFFFFFF4
#define ADDRESS_RESISTIVE_TOUCH_LLY     (unsigned long)0xFFFFFFF2
#define ADDRESS_RESISTIVE_TOUCH_LRX     (unsigned long)0xFFFFFFF0
#define ADDRESS_RESISTIVE_TOUCH_LRY     (unsigned long)0xFFFFFFEE


#ifdef _SFLASH
// define the functions to call for the non-volatile memory
#define NVMSectorErase                  ((NVM_SECTORERASE_FUNC)&DRV_NVM_M25P80_SectorErase)
#define NVMWrite                        ((NVM_WRITE_FUNC      )&DRV_NVM_M25P80_Write)
#define NVMRead                         ((NVM_READ_FUNC       )&DRV_NVM_M25P80_Read)
#else
#define NVMSectorErase                  NULL
#define NVMWrite                        NULL
#define NVMRead                         NULL
#endif

// default touch screen font definition
#define DRV_TOUCHSCREEN_FONT            VeraMono_12

/********************************************************************
 * definitions for micro SD card pins
 ********************************************************************/
#define SD_SPI_CHN      2
#define SD_CS_TRIS      _TRISG9     // Chip Select
#define SD_CS_LAT       _LATG9

#define SD_CD_TRIS      _TRISA6     // Card Detect
#define SD_CD           _RA6

#define SD_WP           false       // no WP available on micro SD cards

#define SD_MAX_FILES    1           // maximum number of files open simultaneously

/********************************************************************
 * definitions for the SPI-Flash pins
 ********************************************************************/
#define SFLASH_SPI_CHN  2
#define M25P80_CS_TRIS   TRISCbits.TRISC2
#define M25P80_CS_LAT    LATCbits.LATC2

//#define SST25_SCK_TRIS  TRISGbits.TRISG6
//#define SST25_SDO_TRIS  TRISGbits.TRISG8
//#define SST25_SDI_TRIS  TRISGbits.TRISG7

#define ERASE_SECTOR_SIZE   65536UL

// SPI driver configuration shared with CODEC
#define fastParameters   3,   6,   0,  SPI_BUS_MODE_2, 0
//                      pri, sec, cke, ckp=1/smp=0, mode16
#define slowParameters   0,   0,   0,  SPI_BUS_MODE_2,  0
//                      pri, sec, cke, ckp=1/smp=0, mode16

/********************************************************************
 * definitions for the VS1053 CODEC pins
 ********************************************************************/
#define CODEC_SPI_CHN             2               // shared with Serial Flash, SD card
#define CODEC_RST_Config()        _TRISA5 = 0     // o reset decoder
#define CODEC_RST_Enable()        _LATA5 = 0
#define CODEC_RST_Disable()       _LATA5 = 1

#define CODEC_DREQ                _RA4            // i request for data

#define CODEC_DCS_Config()        _TRISG12 = 0    // o data select
#define CODEC_DCS_Enable()        _LATG12 = 0
#define CODEC_DCS_Disable()       _LATG12 = 1

#define CODEC_CS_Config()         _TRISA7 = 0     // o command select
#define CODEC_CS_Enable()         _LATA7 = 0
#define CODEC_CS_Disable()        _LATA7 = 1


/*********************************************************************
* HARDWARE PROFILE for USB to program external memory
*********************************************************************/

#define DataChipErase     ((FLASH_CHIPERASE_FUNC)&DRV_NVM_M25P80_ChipErase)
#define DataWrite         ((FLASH_WRITE_FUNC    )&DRV_NVM_M25P80_Write)
#define DataRead          ((FLASH_READ_FUNC     )&DRV_NVM_M25P80_Read)

#define USE_COMM_PKT_MEDIA_USB
#define COMM_PKT_RX_MAX_SIZE    (1024)

//#define USE_SELF_POWER_SENSE_IO
#define tris_self_power     TRISAbits.TRISA2    // Input
#define self_power          1

//#define USE_USB_BUS_SENSE_IO
#define tris_usb_bus_sense  TRISBbits.TRISB5    // Input
#define USB_BUS_SENSE       U1OTGSTATbits.SESVD // Special considerations


#endif




