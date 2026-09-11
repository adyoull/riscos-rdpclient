/*
    ####             #    #     # #
    #   #            #    #       #          The FreeWare C library for
    #   #  ##   ###  #  # #     # ###             RISC OS machines
    #   # #  # #     # #  #     # #  #   ___________________________________
    #   # ####  ###  ##   #     # #  #
    #   # #        # # #  #     # #  #    Please refer to the accompanying
    ####   ### ####  #  # ##### # ###    documentation for conditions of use
    ________________________________________________________________________

    File:    Pane2.FindMain.c
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

extern main_listelement *FindMainWindow(window_handle window)
{
 /*
  * searches through list of main window elements and returns pointer to
  * element for 'window'
  */
  main_listelement *currentelement;

  currentelement = LinkList_FirstItem(&main_anchor);

  while(currentelement != NULL){

     if(currentelement->mainwindow == window)
        break;

     currentelement = LinkList_NextItem(&(currentelement->header));
  }

  return(currentelement);
}
