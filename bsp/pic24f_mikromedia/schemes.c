/*
 * default schemes and utils
 */

#include "schemes.h"
#include "string.h"

void Scheme_Initialize( GFX_GOL_OBJ_SCHEME* style, void * font)
{

    GFX_GOL_SchemeFontSet( style, (void*)font);
    GFX_GOL_SchemeColorSet( style, YELLOW, YELLOW, BLACK);
    GFX_GOL_SchemeEmbossSet( style, DARKGRAY, LIGHTGRAY, 1);
    GFX_GOL_SchemeEmbossSet( style, 0, 0, 0);
    GFX_GOL_SchemeTextColorSet( style, BLACK, WHITE, LIGHTGRAY);
    GFX_GOL_SchemeBackgroundImageSet( style, 0, 0, NULL);
    GFX_GOL_SchemeBackgroundTypeSet(  style, GFX_BACKGROUND_COLOR);
    GFX_GOL_SchemeBackgroundColorSet( style, BLACK);
    GFX_GOL_SchemeFillStyleSet( style, GFX_FILL_STYLE_COLOR);
    GFX_GOL_SchemeGradientColorSet( style,
                            GFX_RGBConvert(0x2B, 0x55, 0x87),
                            GFX_RGBConvert(0xD4, 0xE4, 0xF7));
}


void Scheme_Copy( GFX_GOL_OBJ_SCHEME * dest, GFX_GOL_OBJ_SCHEME* source)
{
    memcpy( (void*)dest, (void*)source, sizeof( GFX_GOL_OBJ_SCHEME));
}
