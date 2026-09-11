/*
    ####             #    #     # #
    #   #            #    #       #          The FreeWare C library for 
    #   #  ##   ###  #  # #     # ###             RISC OS machines
    #   # #  # #     # #  #     # #  #   ___________________________________
    #   # ####  ###  ##   #     # #  #                                      
    #   # #        # # #  #     # #  #    Please refer to the accompanying
    ####   ### ####  #  # ##### # ###    documentation for conditions of use
    ________________________________________________________________________

    File:    TabDialog.h
    Author:  Copyright © 2000 Andrew Sellors.
    Version: 1.01 (12st January 2001)
    Purpose: Creates a 'tabbed dialog' using panes.
    Mods:    1.00 -> 1.01
             Added TabDialog_GetTab
*/

#ifndef __dl_tabdialog_h
#define __dl_tabdialog_h

#ifdef __cplusplus
extern "C" {
#endif

#ifndef __dl_wimp_h
#include "Desklib:Wimp.h"
#endif

#ifndef __dl_window_h
#include "Desklib:Window.h"
#endif

typedef struct {
  window_handle mainwindow;
  icon_handle min_tab;
  icon_handle max_tab;
  icon_handle tab_background;
  int current_tab;
  int tab_count;
  wimp_point pane_offset;
  window_handle tabwindow[1];
} tabdialog_def;

extern tabdialog_def *TabDialog_Create(char *mainwindow,
                                       icon_handle min_tab, icon_handle max_tab,
                                       icon_handle tab_background,
                                       int default_tab,
                                       ...);

extern void TabDialog_Show(tabdialog_def *def, window_openpos openpos);


extern void TabDialog_Hide(tabdialog_def *def);


extern void TabDialog_Delete(tabdialog_def *def);


extern void TabDialog_HandleClick(tabdialog_def *def, icon_handle icon);


extern void TabDialog_SetTab(tabdialog_def *def, int tab);


extern int TabDialog_GetTab(tabdialog_def *def);

#ifdef __cplusplus
}
#endif

#endif
