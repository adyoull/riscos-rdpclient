/*
    ####             #    #     # #
    #   #            #    #       #          The FreeWare C library for
    #   #  ##   ###  #  # #     # ###             RISC OS machines
    #   # #  # #     # #  #     # #  #   ___________________________________
    #   # ####  ###  ##   #     # #  #
    #   # #        # # #  #     # #  #    Please refer to the accompanying
    ####   ### ####  #  # ##### # ###    documentation for conditions of use
    ________________________________________________________________________

    File:    Pane2.CAddMain.c
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

extern window_handle Pane2_CreateAndAddMain(const char *windowname, int maxtitlesize)
{
 /*
  * Creates window as Window_Create and initialises it with the library as a
  * main window that can have panes attached to it.
  * The window handle is returned or 0 if window cannot be created.
  */
  window_handle mainwindow;

  mainwindow = Window_Create(windowname, maxtitlesize);

  if(mainwindow != 0){

     if(!Pane2_AddMain(mainwindow)){

        Window_Delete(mainwindow); /* delete mainwindow if could not be added */
        mainwindow = 0;

     }

  }

  return(mainwindow);
}
