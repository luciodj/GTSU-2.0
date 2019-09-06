/*
 * Module: slice.h
 */

#include <xc.h>
#include <stdint.h>
#include "system_config.h"

#define X( pRct)   (pRct)->left, (pRct)->top, (pRct)->right, (pRct)->bottom

typedef struct{
    uint16_t left;
    uint16_t top;
    uint16_t right;
    uint16_t bottom;
    } Rectangle;

    /**
 * Initializes a rectangle structure to use the entire screen
 * @param pRct  pointer to a rectangle structure to be initialized
 * @return      pointer to the initialized rectangle structure
 */
Rectangle* Slice_Initialize( Rectangle *pRct);


/**
 * Slices off the left % of the source rectangle
 * @param slice     pointer to rectangle sliced off
 * @param source    pointer to the original rectangle
 * @param fract     percentage of area to be removed
 * @return          pointer to the sliced off rectangle
 */
Rectangle* Slice_Left( Rectangle* slice, Rectangle* source, uint8_t fract);
Rectangle* Slice_Bottom( Rectangle* slice, Rectangle* source, uint8_t fract);

/**
 * Extract a rectangle with required alignment from a given slice
 * @param source        pointer to rectangle to be sliced
 * @param mode          char pair, [c- center, l-left, r-right],[ c-center, t-top, b-bottom]
 * @param width         required field width
 * @param height        required field height
 * @return              pointer to slice
 */
Rectangle* Slice_Align( Rectangle* source, char mode[2], uint16_t width, uint16_t height);

