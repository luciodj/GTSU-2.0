/*
 * Module:  slice.c
 *
 * Inspired by: Tkinter packer
 *
 */


#include "slice.h"
#include "gfx/gfx.h"

/**
 * Initializes a rectangle structure to use the entire screen
 * @param pRct  pointer to a rectangle structure to be initialized
 * @return      pointer to the initialized rectangle structure
 */
Rectangle* Slice_Initialize( Rectangle *pRct)
{
    pRct->left = 0;
    pRct->top = 0;
    pRct->right = GFX_MaxXGet();
    pRct->bottom = GFX_MaxYGet();
    return pRct;
} // slice initialize


/**
 * Slices off the left % of the source rectangle
 * @param slice     pointer to rectangle sliced off
 * @param source    pointer to the original rectangle
 * @param fract     percentage of area to be removed
 * @return          pointer to the sliced off rectangle
 */
Rectangle* Slice_Left( Rectangle* slice, Rectangle* source, uint8_t fract)
{
    uint16_t nr = (source->right - source->left) * fract/ 100 + source->left;
    // adjust reminder
    source->left = nr+1;
    // return slice
    slice->left = source->left;
    slice->top = source->top;
    slice->right = nr;
    slice->bottom = source->bottom;
    return slice;
} // slice left


/**
 * Slices off the bottom % of the source rectangle
 * @param slice     pointer to rectangle sliced off
 * @param source    pointer to the original rectangle
 * @param fract     percentage of area to be removed
 * @return          pointer to the sliced off rectangle
 */
Rectangle* Slice_Bottom( Rectangle* slice, Rectangle* source, uint8_t fract)
{
    uint16_t nt = source->bottom - (source->bottom - source->top) * fract/ 100;
    // return slice
    slice->left = source->left;
    slice->top = nt;
    slice->right = source->right;
    slice->bottom = source->bottom;
    // adjust reminder
    source->bottom = nt-1;
    return slice;
} // slice left


/**
 * Extract a rectangle with required alignment from a given slice
 * @param source        pointer to rectangle to be sliced
 * @param mode          char pair, [c- center, l-left, r-right],[ c-center, t-top, b-bottom]
 * @param width         required field width
 * @param height        required field height
 * @return              pointer to slice
 */
Rectangle* Slice_Align( Rectangle* source, char mode[2], uint16_t width, uint16_t height)
{
    uint16_t margin;
    // check horizontal alignment
    margin = (source->right - source->left - width)/2;
    if ( margin > 0)
    {
        switch ( mode[0])
        {
            case 'l':
                source->right -= margin*2;
                break;
            case 'r':
                source->left += margin*2;
                break;
            case 'c':
            default:
                source->left += margin;
                source->right -= margin;
                break;
        } // switch
    }

    // check vertical alignment
    margin = (source->bottom - source->top - height)/2;
    if ( margin >0)
    {
        switch( mode[1])
        {
            case 't':
                source->bottom -= margin*2;
                break;
            case 'b':
                source->top += margin*2;
                break;
            case 'c':
            default:
                source->top += margin;
                source->bottom -= margin;
                break;
        } // switch
    }

    // return resulting rectangle
    return source;
} // slice align
