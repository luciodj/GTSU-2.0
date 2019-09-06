/*
 * Project: 7.3-Slider
 *
 * File:    main.c
 *
 */

#include "system.h"
#include "schemes.h"
#include "slice.h"
#include "fonts/arimo22.h"

bool App_MsgCallback( GFX_GOL_TRANSLATED_ACTION objAct,
                    GFX_GOL_OBJ_HEADER* pObj,
                    GFX_GOL_MESSAGE* pMsg)
{
    if( pObj->ID == 2)                  // intercept messages from the slider
    {
        // update the screen backlight
        SYSTEM_BacklightSet( GFX_GOL_ScrollBarPositionGet( (void*)pObj));
    }

#ifdef _SCREENCAPTURE
    if ( pObj->ID == 2)
    {
        FSInit();
        ScreenCapture( "name.scr");
    }
#endif

    return 1;
} // GOL Msg Callback


bool App_DrawCallback()
{
    return 1;
} // GOL Draw Callback


int main( void)
{
    GFX_GOL_MESSAGE msg;
    GFX_GOL_OBJ_SCHEME WinStyle, SlideStyle;
    Rectangle rMain, rSlide;

    // 1. initializations
    SYSTEM_BoardInitialize();
    SYSTEM_BacklightInitialize();               // init to full brightness

    // 2. create new color schemes
    Scheme_Initialize( &WinStyle, (void*)&Arimo_Regular_22);

    Scheme_Initialize( &SlideStyle, NULL);
    GFX_GOL_SchemeColor0Set( &SlideStyle, DARKGRAY);
    GFX_GOL_SchemeFillStyleSet( &SlideStyle, GFX_FILL_STYLE_GRADIENT_DOUBLE_HOR);

    // 3. create the window object (banner)
    Slice_Initialize( &rMain);
    GFX_GOL_WindowCreate(  1,                   // unique ID
                X( &rMain),
                GFX_GOL_WINDOW_DRAW_STATE,
                NULL,                           // icon
                "Slider Demo",                  // window title
                GFX_ALIGN_CENTER,               // center title horizontally
                &WinStyle                       // default style scheme
            );

    // 4. create a slider object
    Slice_Bottom( &rSlide, &rMain, 85);
    Slice_Align( &rSlide, "cc", 260, 50);
    GFX_GOL_ScrollBarCreate(  2,                // unique ID
                X( &rSlide),              // position and size
                GFX_GOL_SCROLLBAR_SLIDER_MODE_STATE |
                GFX_GOL_SCROLLBAR_DRAW_STATE,   // state
                250,                            // 0..range
                25,                             // resolution step (page)
                250,                            // initial position (100%)
                &SlideStyle                       // default style scheme
            );

    // 5. GOL main loop
    GFX_GOL_MessageCallbackSet( App_MsgCallback);
    GFX_GOL_DrawCallbackSet( App_DrawCallback);
    
    while( 1)
    {
        if ( GFX_GOL_ObjectListDraw())  // if done drawing the objects
        {
            TouchGetMsg( &msg);         // generate a messsage if touched
            GFX_GOL_ObjectMessage( &msg);              // process the message
        }
    } // main interface loop
} // main

