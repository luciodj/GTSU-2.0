/*
 * File:   grid.h
 *
 *
 * Requires: MLA 1312
 */

#ifndef  _GRID_H
#define  _GRID_H

#include "system.h"
#include "tty.h"

/**
 */
typedef struct
{
    unsigned      x: 4;     // x coordinate on the touch grid
    unsigned      y: 4;     // y coordinate on the touch grid
    unsigned  valid: 1;     // flag, indicates a valid touch event detected
    unsigned option: 1;     // flag, can be use to capture long touch events
} code_t;


/**
 * @brief   Initializes the Resistive touch interface (uses Timer3) and defines
 *              a grid to simplify touch input
 *
 * @param x  dimension of the horizontal grid (example: GetMaxX()/4)
 * @param y  dimension of the vertical grid (example: GetMaxY()/4)
 */
void Grid_Init( int x, int y);


/**
 * @brief   Fetches current touch state machine output and packs it in t_code 
 *              structure 
 *
 * @return  t_code containing current status
 */
code_t Grid_TouchGet( void);


/**
 * @brief   Blocking wait for a touch and release event. Debounces and detects
 *              event duration (setting the option flag)
 * @return
 */
code_t Grid_ButtonGet( void);


/**
 * @brief   set the horizontal and vertical dimensions of the touch grid
 *
 */
void Grid_SizeSet( int x, int y);

#endif
