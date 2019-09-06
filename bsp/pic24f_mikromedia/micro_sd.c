/*
 * File:    micro_sd.c
 *
 * Simplified file system support for Mikromedia boards
 *
 */

#include "micro_sd.h"
#include "system_config.h"
#include "fileio/fileio.h"
#include "driver/fileio/sd_spi.h"

/*********************************************************************
 * SD card - CONTROLLER INTERFACE
 *********************************************************************/
void _SpiConfigurePins (void)
{
    // Deassert the chip select pin
    SD_CS_LAT = 1;
    // Configure CS pin as an output
    SD_CS_TRIS = 0;
    // Configure CD pin as an input (default)
    //_TRISA6 = 1;
    // Configure WP pin as an input NA (default)
    //_TRIS = 1;
}

inline void _SpiSetCs(uint8_t a)
{
    SD_CS_LAT = a;
}

inline bool _SpiGetCd(void)
{
    return (!SD_CD) ? true : false;
}

inline bool _SpiGetWp(void)
{
    return SD_WP;
}

const FILEIO_SD_DRIVE_CONFIG _MediaParameters =
{
    SD_SPI_CHN,             // Use SPI module 2
    _SpiSetCs,             // set/clear the Chip Select pin.
    _SpiGetCd,             // get the status of the Card Detect pin.
    _SpiGetWp,             // get the status of the Write Protect pin.
    _SpiConfigurePins      // configure the pins' TRIS bits.
};

// default stubs
void __attribute__((weak)) FILEIO_SD_IOInitialize(FILEIO_SD_DRIVE_CONFIG * config){};
bool __attribute__((weak))  FILEIO_SD_MediaDetect(FILEIO_SD_DRIVE_CONFIG * config){ return false;};
FILEIO_MEDIA_INFORMATION * __attribute__((weak)) FILEIO_SD_MediaInitialize(FILEIO_SD_DRIVE_CONFIG * config){return NULL;};
bool  __attribute__((weak))  FILEIO_SD_MediaDeinitialize(FILEIO_SD_DRIVE_CONFIG * config){ return false;};
bool  __attribute__((weak)) FILEIO_SD_SectorRead(FILEIO_SD_DRIVE_CONFIG * config, uint32_t sector_addr, uint8_t * buffer){ return false;};
bool  __attribute__((weak)) FILEIO_SD_SectorWrite(FILEIO_SD_DRIVE_CONFIG * config, uint32_t sector_addr, uint8_t * buffer, bool allowWriteToZero){ return false;};
bool  __attribute__((weak)) FILEIO_SD_WriteProtectStateGet(FILEIO_SD_DRIVE_CONFIG * config) { return false;};


// The gSDDrive structure allows the user to specify which set of driver functions should be used by the
// FILEIO library to interface to the drive.
// This structure must be maintained as long as the user wishes to access the specified drive.
const FILEIO_DRIVE_CONFIG _Drive =
{
    (FILEIO_DRIVER_IOInitialize)FILEIO_SD_IOInitialize,                     // Function to initialize the I/O pins used by the driver.
    (FILEIO_DRIVER_MediaDetect)FILEIO_SD_MediaDetect,                       // Function to detect that the media is inserted.
    (FILEIO_DRIVER_MediaInitialize)FILEIO_SD_MediaInitialize,               // Function to initialize the media.
    (FILEIO_DRIVER_MediaDeinitialize)FILEIO_SD_MediaDeinitialize,           // Function to de-initialize the media.
    (FILEIO_DRIVER_SectorRead)FILEIO_SD_SectorRead,                         // Function to read a sector from the media.
    (FILEIO_DRIVER_SectorWrite)FILEIO_SD_SectorWrite,                       // Function to write a sector to the media.
    (FILEIO_DRIVER_WriteProtectStateGet)FILEIO_SD_WriteProtectStateGet,     // Function to determine if the media is write-protected.
};

bool    files_lock[ SD_MAX_FILES];
SD_FILE files[ SD_MAX_FILES];

SD_FILE *_files_avail_get( void)
{
    int i;
    for( i=0; i<SD_MAX_FILES; i++)
    {
        if ( !files_lock[i])
        {
            files_lock[i]=true;
            return &files[i];
        }
    }

    // return error, all file objects are used up
    return NULL;
}

/*********************************************************************
 * Simplified API for media access
 *********************************************************************/

bool SD_Initialize( void)
{
    int i;
    // close all files and free them up
    for( i=0; i<SD_MAX_FILES; i++)
        files_lock[i] = false;

    FILEIO_Initialize();

    return FILEIO_DriveMount( 'A', &_Drive, (void*)&_MediaParameters);
} // SD_Initialize


SD_FILE *SD_Open( char *path, char *attr)
{
    FILEIO_OBJECT *fp = NULL;
    uint16_t iattr = 0;

    // find an available file object
    if ( (fp = _files_avail_get()) == NULL)
        return NULL;        // close files left open or increase the SD_MAX_FILES in system_config

    // convert string attr to integers
    switch( *attr)
    {
        case 'w':
            iattr |= FILEIO_OPEN_WRITE;
            break;
        case 'a':
            iattr |= FILEIO_OPEN_APPEND;
            break;
        case 'r':
            iattr |= FILEIO_OPEN_READ;
            break;
    }
    // TODO: add + modifiers

    // attempt to open the file
    if ( FILEIO_Open( fp, path, iattr))
        return NULL;

    return fp;
} // SD_Open

//
//size_t SD_Read( void * buffer, size_t size, size_t count, SD_FILE *fp )
//{
//    return FILEIO_Read( buffer, size, count, fp);
//}
//
//
//size_t SD_Write( void *buffer, size_t size, size_t count, SD_FILE *fp)
//{
//    return FILEIO_Write( buffer, size, count, fp);
//}
//
//
void SD_Close( SD_FILE *fp)
{
    files_lock[ (files-fp)/sizeof(void*)] = false;
    FILEIO_Close( fp);
}

