/*
    ####             #    #     # #
    #   #            #    #       #          The FreeWare C library for
    #   #  ##   ###  #  # #     # ###             RISC OS machines
    #   # #  # #     # #  #     # #  #   ___________________________________
    #   # ####  ###  ##   #     # #  #
    #   # #        # # #  #     # #  #    Please refer to the accompanying
    ####   ### ####  #  # ##### # ###    documentation for conditions of use
    ________________________________________________________________________

    File:    KeyCodes.h
    Author:  Copyright © 1992 Jason Williams
    Version: 1.00 (24 Jan 1992)
    Purpose: Defines all the Wimp-returned keycodes of special keys
             (return, function keys, etc.), including the values
             returned if shift/ctrl are also held down.
*/


#ifndef __dl_keycodes_h
#define __dl_keycodes_h

#ifdef __cplusplus
extern "C" {
#endif


/* Abstract */
/*
  This defines all the Wimp-returned keycodes of special keys (return,
  function keys, etc), including the values returned if shift/ctrl
  are also held down.

  Some of the codes are the same with shift or control pressed.  These
  variants' codes are set to UNDEFINED.
*/


#define UNDEFINED -1
/*
  Some keypresses are returned the same if shift, ctrl are held down, so we
  don't want people to think that keycode_SHIFT_X returns a differnt
  value to keycode_X.  Instead, we set these codes to UNUSED.
*/


/* Keys on their own ------------------------------------------------------- */

#define keycode_BACKSPACE   8
#define keycode_RETURN      13
#define keycode_ESCAPE      27
#define keycode_HOME        30
#define keycode_DELETE      127

#define keycode_F1          385
#define keycode_F2          386
#define keycode_F3          387
#define keycode_F4          388
#define keycode_F5          389
#define keycode_F6          390
#define keycode_F7          391
#define keycode_F8          392
#define keycode_F9          393

#define keycode_PRINT       384
#define keycode_TAB         394
#define keycode_COPY        395

#define keycode_CURSORLEFT  396
#define keycode_CURSORRIGHT 397
#define keycode_CURSORDOWN  398
#define keycode_CURSORUP    399
#define keycode_PAGEDOWN    414
#define keycode_PAGEUP      415

#define keycode_F10         458
#define keycode_F11         459
#define keycode_F12         460

#define keycode_INSERT      461



/* Keys in combination with SHIFT ------------------------------------------ */

#define keycode_SHIFT_BACKSPACE   UNDEFINED
#define keycode_SHIFT_RETURN      UNDEFINED
#define keycode_SHIFT_ESCAPE      UNDEFINED
#define keycode_SHIFT_HOME        UNDEFINED
#define keycode_SHIFT_DELETE      UNDEFINED

#define keycode_SHIFT_F1          401
#define keycode_SHIFT_F2          402
#define keycode_SHIFT_F3          403
#define keycode_SHIFT_F4          404
#define keycode_SHIFT_F5          405
#define keycode_SHIFT_F6          406
#define keycode_SHIFT_F7          407
#define keycode_SHIFT_F8          408
#define keycode_SHIFT_F9          409

#define keycode_SHIFT_PRINT       400
#define keycode_SHIFT_TAB         410
#define keycode_SHIFT_COPY        411

#define keycode_SHIFT_CURSORLEFT  412
#define keycode_SHIFT_CURSORRIGHT 413
#define keycode_SHIFT_CURSORDOWN  414
#define keycode_SHIFT_CURSORUP    415
#define keycode_SHIFT_PAGEDOWN    398
#define keycode_SHIFT_PAGEUP      399

#define keycode_SHIFT_F10         474
#define keycode_SHIFT_F11         475
#define keycode_SHIFT_F12         476

#define keycode_SHIFT_INSERT      477



/* Keys in combination with CTRL ------------------------------------------- */

#define keycode_CTRL_BACKSPACE   UNDEFINED
#define keycode_CTRL_RETURN      UNDEFINED
#define keycode_CTRL_ESCAPE      UNDEFINED
#define keycode_CTRL_HOME        UNDEFINED
#define keycode_CTRL_DELETE      UNDEFINED

#define keycode_CTRL_F1          417
#define keycode_CTRL_F2          418
#define keycode_CTRL_F3          419
#define keycode_CTRL_F4          420
#define keycode_CTRL_F5          421
#define keycode_CTRL_F6          422
#define keycode_CTRL_F7          423
#define keycode_CTRL_F8          424
#define keycode_CTRL_F9          425

#define keycode_CTRL_PRINT       416
#define keycode_CTRL_TAB         426
#define keycode_CTRL_COPY        427

#define keycode_CTRL_CURSORLEFT  428
#define keycode_CTRL_CURSORRIGHT 429
#define keycode_CTRL_CURSORDOWN  430
#define keycode_CTRL_CURSORUP    431
#define keycode_CTRL_PAGEDOWN    446
#define keycode_CTRL_PAGEUP      447

#define keycode_CTRL_F10         490
#define keycode_CTRL_F11         491
#define keycode_CTRL_F12         492

#define keycode_CTRL_INSERT      493

#define keycode_CTRL_A             1
#define keycode_CTRL_B             2
#define keycode_CTRL_C             3
#define keycode_CTRL_D             4
#define keycode_CTRL_E             5
#define keycode_CTRL_F             6
#define keycode_CTRL_G             7
#define keycode_CTRL_H             8
#define keycode_CTRL_I             9
#define keycode_CTRL_J            10
#define keycode_CTRL_K            11
#define keycode_CTRL_L            12
#define keycode_CTRL_M            13
#define keycode_CTRL_N            14
#define keycode_CTRL_O            15
#define keycode_CTRL_P            16
#define keycode_CTRL_Q            17
#define keycode_CTRL_R            18
#define keycode_CTRL_S            19
#define keycode_CTRL_T            20
#define keycode_CTRL_U            21
#define keycode_CTRL_V            22
#define keycode_CTRL_W            23
#define keycode_CTRL_X            24
#define keycode_CTRL_Y            25
#define keycode_CTRL_Z            26

/* Keys in combination with SHIFT & CTRL ----------------------------------- */

#define keycode_CTRL_SHIFT_BACKSPACE   UNDEFINED
#define keycode_CTRL_SHIFT_RETURN      UNDEFINED
#define keycode_CTRL_SHIFT_ESCAPE      UNDEFINED
#define keycode_CTRL_SHIFT_HOME        UNDEFINED
#define keycode_CTRL_SHIFT_DELETE      UNDEFINED

#define keycode_CTRL_SHIFT_F1          0x1B1
#define keycode_CTRL_SHIFT_F2          0x1B2
#define keycode_CTRL_SHIFT_F3          0x1B3
#define keycode_CTRL_SHIFT_F4          0x1B4
#define keycode_CTRL_SHIFT_F5          0x1B5
#define keycode_CTRL_SHIFT_F6          0x1B6
#define keycode_CTRL_SHIFT_F7          0x1B7
#define keycode_CTRL_SHIFT_F8          0x1B8
#define keycode_CTRL_SHIFT_F9          0x1B9

#define keycode_CTRL_SHIFT_PRINT       0x1B0
#define keycode_CTRL_SHIFT_TAB         0x1BA
#define keycode_CTRL_SHIFT_COPY        0x1BB

#define keycode_CTRL_SHIFT_CURSORLEFT  0x1BC
#define keycode_CTRL_SHIFT_CURSORRIGHT 0x1BD
#define keycode_CTRL_SHIFT_CURSORDOWN  0x1BE
#define keycode_CTRL_SHIFT_CURSORUP    0x1BF
#define keycode_CTRL_SHIFT_PAGEDOWN    0x1AE
#define keycode_CTRL_SHIFT_PAGEUP      0x1AF

#define keycode_CTRL_SHIFT_F10         0x1FA
#define keycode_CTRL_SHIFT_F11         0x1FB
#define keycode_CTRL_SHIFT_F12         0x1FC

#define keycode_CTRL_SHIFT_INSERT      0x1FD

#ifdef __cplusplus
}
#endif

#endif
