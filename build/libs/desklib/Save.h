/*
    ####             #    #     # #
    #   #            #    #       #          The FreeWare C library for
    #   #  ##   ###  #  # #     # ###             RISC OS machines
    #   # #  # #     # #  #     # #  #   ___________________________________
    #   # ####  ###  ##   #     # #  #
    #   # #        # # #  #     # #  #    Please refer to the accompanying
    ####   ### ####  #  # ##### # ###    documentation for conditions of use
    ________________________________________________________________________

    File:    Save.h
    Author:  Copyright © 1994 Julian Smith and Jason Howat
    Version: 1.03 (17 Apr 2005)
    Purpose: Automate handling of save-as windows
    Mods:    13-Jun-1994 - JPS - Added filetype handling
             18-Jun-1994 - JDH - See Save.c
*/

#ifndef __dl_save_h
#define __dl_save_h

#include <stdlib.h> /* For size_t */

#ifndef __dl_event_h
#include "DeskLib:Event.h"
#endif


#ifdef __cplusplus
extern "C" {
#endif


/* Abstract */
/*
  This header defines functions to help automate the handling of save
  windows, by dealing with dragging icons to directories and the various
  datasave protocol messages.  You register handlers to deal with the
  actual saving of data to file or ramtransfers to other applications.
*/


typedef BOOL (*save_filesaver)(char *filename, void *ref);
/*
  This type of function should save data to the given filename, and
  return TRUE if successful or FALSE otherwise.  'ref' is the reference
  originally passed to Save_InitSaveWindowHandler.
*/


typedef int (*save_ramsaver)(task_handle  sourcetask,  /* our task handle */
                             void         *ref,
                             task_handle  desttask,
                             void         *destbuffer,
                             unsigned int buffersize,
                             int          progress);
/*
  This type of function should use Wimp_TransferBlock to place data in the
  given destination buffer belonging to the destination task.

  The 'sourcetask' is our task handle, given here for convenience.
  'ref' is the reference originally passed to Save_InitSaveWindowHandler and
  'progress' is the number of bytes transferred so far.

  You should return the number of bytes you have transferred into
  'destbuffer', or a negative number if an error has occurred.

  As the task requesting the data can ask for it in as many parts as are
  needed to transfer the data, the sequence of messages will continue
  until you return less than a full buffer or the destination task decides
  it is all transferred.  ie. if you return a value less than 'buffersize'
  then that is the last transfer of the save.
*/


typedef enum
{
  save_SAVEOK    = 0,
  save_RECEIVERFAILED,
  save_FILESAVERFAILED,
  save_RAMSAVERFAILED
} save_result;
/*
  The various results that are possible from an attempt to save.
  Either it went successfully, or it failed in one of three ways -
  the receiver failed to load the scrap file, the file-saver failed
  or the ram-saver failed.
*/


typedef void (*save_resulthandler)(save_result result, void *ref);
/*
  This type of function is called after any attempt to save,
  giving the result of the attempt.

  This is so that your app doesn't go away thinking data has been saved
  when it hasn't.  Note that your saver functions will not be aware of any
  problem in the second case, where the receiver fails to load the scrap
  file.
*/


typedef struct
{
  window_handle    window;        /*  Window that the save icons are in */

  union
  {
    unsigned int  value;
    struct
    {
      unsigned int  is_menu         : 1;  /* Savewindow is part of menu  */
      unsigned int  is_save_window  : 1;  /* Close window after save?    */
      unsigned int  we_are_dragging : 1;  /* Are we dragging file icon?  */
      unsigned int  quit_after_save : 1;  /* Click/drag was with Select  */
      unsigned int  release_after   : 1;  /* Release all handlers when   */
                                          /* the window/menu is closed?  */
      unsigned int  padding         : 27;
    } data;
  } flags;

  icon_handle         dragsprite;
  icon_handle         okbutton;
  icon_handle         cancelbutton;
  icon_handle         filenameicon;
  save_filesaver      filesaver;
  save_ramsaver       ramsaver;
  save_resulthandler  resulthandler;
  size_t              estimatedsize;
  int                 filetype;
  void                *ref;
  int                 ram_progress;          /* Num of bytes ram-transfered. */
  unsigned int        last_message_ref;      /* So we know which incoming    */
                                             /* messages are replies to us   */
} save_saveblock;
/*
  This struct holds all the information needed to handle saving using the RISC
  OS data transfer protocol.  It is used internally by the Save functions,
  and so you shouldn't change most of the fields.  Those that can be changed
  after a successful initial call to Save_InitSaveWindowHandler are given below.

  'ref', 'filesaver', 'ramsaver', 'resulthandler' and 'estimatedsize'  can all
  be changed, for instance in response to the user choosing to save a different
  format of file with a click on a radio button.  If you do decide to change
  them in this way, you can use Save_SetFiletype to change the stored filetype
  and the file icon to match if necessary.

  You can also set the flags 'is_menu' and 'is_save_window', for instance if you
  change a menu-leaf savewindow into a free-standing save window.
*/


save_saveblock *Save_InitSaveWindowHandler(
  window_handle      window,           /* handle of the window to deal with  */
  BOOL               is_menu,          /* is this window part of a menu?     */
                                       /* if TRUE Save_* will close it after */
                                       /* 'Select'-saves                     */
  BOOL               is_save_window,   /* is this window just a save window? */
                                       /* if TRUE Save_* will close it after */
                                       /* 'Select'-saves                     */
  BOOL               release_after,    /* release all Save_ handlers when    */
                                       /* window/menu is closed?             */
  icon_handle        dragsprite,       /* handle of the dragable file-icon   */
  icon_handle        okbutton,         /* handle of 'OK' or 'Save' button    */
  icon_handle        cancelbutton,     /* handle of the 'Cancel' button      */
  icon_handle        filenameicon,     /* handle of the writable fname icon  */
  save_filesaver     filesaver,        /* fn which saves data to a file      */
  save_ramsaver      ramsaver,         /* fn which does Wimp_TransferBlocks  */
  save_resulthandler resulthandler,    /* fn to report success of save to    */
  size_t             estimatedsize,    /* size of data  (estimate)           */
  int                filetype,
  void      *ref                       /* ref passed to all saver functions  */
  );
/*
  You call this function to set up the given window to handle the RISC OS
  save protocol automatically for you.

  You pass in handlers which are called when needed for saving to file,
  doing direct RAM transfers and to receive the result of the attempted
  save.  All these handler functions will be passed the reference 'ref'
  that you specify here, so that they know what needs saving.  The
  ramsaver and resulthandler are both optional, you can pass NULL rather
  than supply functions.  If you don't specify a resulthandler, a default
  one will be used which will report any problems saving using Error_Report.

  You also give a filetype and an estimated size for the data, to be used
  in the save protocol messages.  These should be as accurate as possible.

  You specify an OK button, a cancel button and a draggable icon to hold the
  "filetype" sprite.  This should be an indirected icon (either text or sprite)
  with a buffer at least 9 characters long.  You also pass the handle of a
  writable icon to hold the filename - this should be big enough to hold
  the the full pathname and start with a default leafname to be used for
  the file.  You can leave out any of these except the filenameicon, in which
  case you whould pass a negative number instead of an icon handle for
  those parameters.

  You can also choose how the library deals with your window and the handlers
  attached to it.  Setting 'release_after' to TRUE means that when the window
  is closed (by a click on OK or cancel or by the menu it is part of closing)
  all the handlers attached by this function will be released, ready for you
  to reattach again next time it is opened.  Alternatively, you can set it to
  FALSE and handle the detaching yourself (with Save_ReleaseSaveHandlers) or
  else use the one window for all you saving and juse change the handlers, etc.
  as required.  See the definition of a save_saveblock to see what can be
  changed directly by you.

  Finally, you can set 'is_menu' to TRUE if the window is part of menu structure
  (eg. as a submenu), or FALSE if it is a free-standing window.  This enables
  "Save" to close the menu or window appropriately when you click on "Cancel" or
  "OK" with the Select button.  You set 'is_save_window' to TRUE if you want the
  window closed on Select clicks on OK, and FALSE if you want it left open
  (eg. if the window is not just a save window but also has other information
  deisplayed).

  You are responsible for opening the window and dealing with any menus, radio
  buttons, etc. in the window - everything except the actual dragging, clicking
  on OK or Cancel and the data transfer protocol.  You can change the handlers,
  reference and so on depending on these other clicks - see the definition of a
  save_saveblock to see what can be changed directly by you.
*/


void Save_ReleaseSaveHandlers(save_saveblock *saveblock);
/*
  You only need to use this if you choose not to have the Save library
  remove handlers for you automatically when the window or menu closes
  (ie. if you pass FALSE for release_after when initialising the window).

  The library also doesn't detect "Escape" being pressed in a free-standing
  save window, so if you want to use that to close the window, you'll
  presumably want to call this afterwards to release the handlers.
*/


void Save_SetFiletype(save_saveblock *saveblock, int filetype);
/*
  Sets the sprite in the savewindow to be "file_..." and also alters the
  saveblock so that the filetype is used whenever date is saved to the
  filer or another application.

  This can only be called after the saveblock has been initialised with
  a successful call to Save_InitSaveWindowHandler.
*/


#ifdef __cplusplus
}
#endif


#endif
