/*
    ####             #    #     # #
    #   #            #    #       #          The FreeWare C library for
    #   #  ##   ###  #  # #     # ###             RISC OS machines
    #   # #  # #     # #  #     # #  #   ___________________________________
    #   # ####  ###  ##   #     # #  #
    #   # #        # # #  #     # #  #    Please refer to the accompanying
    ####   ### ####  #  # ##### # ###    documentation for conditions of use
    ________________________________________________________________________

    File:    Pane2.ReadIcon.c
    Author:  Copyright © 1995 Andrew Sellors.
    Version: 1.04 (4th August 1995)
    Purpose: Handles windows with panes.
*/

#include "DeskLib:Pane2.h"
#include "Pane2Defs.h"
#include "Desklib:Event.h"
#include "DeskLib:Template.h"
#include "Desklib:EventMsg.h"
#include "Desklib:Error.h"

#include <stdlib.h>


/******************************************************************************/

extern void Pane2_ReadIcon(window_handle window, icon_handle icon, const char *panewindow,
                           wimp_point *paneoffset, wimp_point *panesize)
{
 /*
  * Calculates the pane offset and size so that the pane will be in the same
  * position and size as the icon in the window.
  * If 'panewindow' is the name of the pane window in the template file then the
  * presence of any scroll bars or the window title bar is taken into accound
  * when calculating the position and size of the pane, set it to NULL and no
  * action is taken.
  * Need to use pane2_MAINTOP and pane2_PANETOP flags.
  */
  icon_block iconinfo;
  window_block *block;

  Wimp_GetIconState(window, icon, &iconinfo);

  panesize->x = iconinfo.workarearect.max.x - iconinfo.workarearect.min.x;
  panesize->y = iconinfo.workarearect.max.y - iconinfo.workarearect.min.y;

  paneoffset->x = iconinfo.workarearect.min.x;
  paneoffset->y = -iconinfo.workarearect.max.y;

 /*
  * Try and correct size to take into acount scroll bars etc.
  * Should use difference between window outline and window openpos to find
  * tool size but this requires the window to be already open.
  */
  if(panewindow != NULL){

     block = Template_Find(panewindow);

     if(block != NULL){

        if(block->flags.data.hastitle || block->flags.data.titlebar){ /* title bar */

           paneoffset->y += tool_SIZE;
           panesize->y -= tool_SIZE;

        }

        if(block->flags.data.hasvscroll || block->flags.data.vscroll){ /* vert scroll */

           panesize->x -= tool_SIZE;

        }

        if(block->flags.data.hashscroll || block->flags.data.hscroll){ /* horz scroll */

           panesize->y -= tool_SIZE;

        }

     }

  }
}
