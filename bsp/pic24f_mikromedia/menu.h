/*
 * File:   menu.h
 *
 *
 * Requires: MLA 2013-12
 */

#include "system_config.h"
#include "tty.h"
#include "grid.h"
#include "fileio/fileio.h"

#define N_ITEMS     12      // maximum length of selection list

/**
 */
int Menu_SimpleMenu( char list[][16], int items);


/**
 */
void Menu_FileSelect( char *fn, char *ext);

