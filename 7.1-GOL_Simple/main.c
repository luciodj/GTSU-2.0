/*
 * Project: 7.1-GOL_Simple
 *
 * File:    main.c
 *
 */
#include "system.h"
#include "fonts/arimo22.h"

bool APP_MsgCallback( GFX_GOL_TRANSLATED_ACTION objAct,
                      GFX_GOL_OBJ_HEADER* pObj,
                      GFX_GOL_MESSAGE* pMsg)
{
//    FSInit();
//    ScreenCapture( "7-Simple.scr");
    return 1;
} // GOL Msg Callback

bool APP_DrawCallback()
{
    return 1;
} // GOL Draw Callback


GFX_GOL_OBJ_SCHEME AppStyle;

int main( void)
{
    GFX_GOL_MESSAGE msg;

    AppStyle.CommonBkColor = BLACK;
    AppStyle.Color0 = YELLOW;
    AppStyle.TextColor0 = BLACK;
    AppStyle.TextColor1 = BLACK;
    AppStyle.EmbossDkColor = BLACK;
    AppStyle.EmbossLtColor = BLACK;
    GFX_GOL_SchemeFontSet( &AppStyle, (void*)&Arimo_Regular_22);

    SYSTEM_BoardInitialize();
    SYSTEM_BacklightInitialize();

    GFX_GOL_ButtonCreate(  1,               // unique ID
                100, 80, 220, 160,          // position and size
                5,                          // radius
                GFX_GOL_BUTTON_DRAW_STATE,  // state
                NULL, NULL,                 // no bitmaps
                "Done",                     // text
                GFX_ALIGN_CENTER,           // alignment
                &AppStyle                   // default style scheme
            );

    GFX_GOL_MessageCallbackSet( APP_MsgCallback);
    GFX_GOL_DrawCallbackSet( APP_DrawCallback);

    while( 1)
    {
        if ( GFX_GOL_ObjectListDraw())      // if done drawing the objects
        {
            TouchGetMsg( &msg);             // generate a messsage if touched
            GFX_GOL_ObjectMessage( &msg);   // process the message
        }

    } // main interface loop
    
} // main



