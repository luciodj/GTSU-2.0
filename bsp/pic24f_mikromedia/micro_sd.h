/* 
 * File:   micro_sd.h
 *
 * A simplified file system for MIkromedia boards
 */

#ifndef MICRO_SD_H
#define	MICRO_SD_H

#include "fileio/fileio.h"

#define SD_FILE    FILEIO_OBJECT


bool   SD_Initialize( void);
SD_FILE * SD_Open( char * path, char* attr);
void   SD_Close( SD_FILE * fp);
#define SD_Read     FILEIO_Read
#define SD_Write    FILEIO_Write
#define SD_Eof      FILEIO_Eof

#endif	/* MICRO_SD_H */

