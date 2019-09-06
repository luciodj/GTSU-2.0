/*
 * File:   vs1053.h
 *
 * Requires: MLA 1312
 */

#ifndef _VS1053_H
#define _VS1053_H

#include "system.h"
#include "system_config.h"

#define CODEC_NORMAL 0          // normal operation
#define CODEC_TEST   0x20       // allow test sequences

// prototypes

void CODEC_Init( uint16_t mode);
void CODEC_VolumeSet( uint16_t left, uint16_t right);
void CODEC_SineTest( uint8_t n);
void CODEC_TestExit( void);
void CODEC_Feed( unsigned char **, size_t *);
void CODEC_Flush( void);

//
#endif
