/*
    ####             #    #     # #
    #   #            #    #       #          The FreeWare C library for
    #   #  ##   ###  #  # #     # ###             RISC OS machines
    #   # #  # #     # #  #     # #  #   ___________________________________
    #   # ####  ###  ##   #     # #  #
    #   # #        # # #  #     # #  #    Please refer to the accompanying
    ####   ### ####  #  # ##### # ###    documentation for conditions of use
    ________________________________________________________________________

    File:    WimpSWIs.h
    Author:  Copyright © 1992, 1993, 1994 John Winters and Jason Williams
    Version: 1.08 (17 Apr 2005)
    Purpose: Prototypes for Wimp SWI calls defined in the Wimp library
             (Basic SWI-level interface to the Wimp)

    Mods:    19 Mar 1995 Julian Smith
             Added macros to make calls to Wimp_Poll(Idle) actually
             call Straylight equivalents, when compilataion is for dynamic
             linking.
             08 Jul 1995 - JPS - Removed #include of kernel.h, and changed
                                 Wimp_SpriteOp prototype, to make
                                 DeskLib usable without Acorn headers

             13 Jul 1995 Sergio Monesi
             Added Wimp_StartTask3 and Wimp_GetWindowInfoNoIcons (for
             RISC OS 3.10 or later).

*/


#ifndef __dl_wimpswis_h
#define __dl_wimpswis_h

#ifndef __dl_core_h
#include "DeskLib:Core.h"
#endif

/*
#ifndef __kernel_h
#include "DeskLib:kernel.h"
#endif
*/

#ifndef __dl_wimp_h
#include "DeskLib:Wimp.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif

/* Abstract */
/*
  This header provides function prototypes for a low-level
  interface to the Wimp SWIs.
*/


/* Task handling: init, closedown, messages, transfers, etc. */

extern os_error *Wimp_Initialise(unsigned int *version,
                                 const char   *name,
                                 task_handle  *task,
                                 int          *messages);
/*
  This function registers a task with the Wimp, and must be called once and
  once only when the task starts up.

  'version' points to the minimum Wimp version needed for this task (* 100),
  'name' points to a short description of the task which appears in the
  task manager.  'mess' points to an array of message numbers the task wants
  to receive (zero-terminated).  This only applies for RISC OS versions
  300 and later.

  The actual Wimp version is returned in 'version', and the task handle in
  'task'.
*/


extern os_error *Wimp_CloseDown(task_handle task);
/*
  This function must be called immediately before the program finishes.
  'task' should be the handle of your task.
*/


extern os_error *Wimp_StartTask(const char *command);
/*
  This function is used to start a task from within a program.
  The text in 'command' can be any * command which will case a Wimp
  program to be executed.  Control will be returned to your program when
  the new task exits, or when it first calls Wimp_Poll.
*/


extern os_error *Wimp_StartTask3(const char *command, task_handle *newtask);
/*
  This function is used to start a task from within a program.
  The text in 'command' can be any * command which will case a Wimp
  program to be executed.  Control will be returned to your program when
  the new task exits, or when it first calls Wimp_Poll.

  This returns the task handle of the started task if it is still
  alive, or zero if it isn't.
*/


extern os_error *Wimp_SlotSize(int *currentslot, int *nextslot, int *freepool);
/*
  This function can be used to read or set the size of the current slot
  (ie. your program's slot size) and the next slot (the default slot size).
  To alter one of the values, invoke the function with the parameter set
  to the value you wish to set it to (you cannot alter the free space).

  If you just want to read the current value, set the variables to -1
  instead.  On exit, the new values will be returned.
*/


extern os_error *Wimp_TransferBlock(task_handle  sourcetask,
                                    void         *sourcebuffer,
                                    task_handle  desttask,
                                    void         *destbuffer,
                                    unsigned int length);
/*
  This function copies a block of memory from the 'sourcebuffer' in the
  source tasks's address space to the 'destbuffer' in the destination
  task's workspace.  The amount of data to transfer (in bytes) is given
  in 'length'.
*/


extern os_error *Wimp_ReportError(os_error *error, int flags, const char *name);
/*
  This reports an error using a standard Wimp non-multitasking error box.
  'name' is the name of the task reporting the error, and should be less
  than twenty character. 'err' should contain the details of the error.
  'flags' is passed in as an int, but can be cast from an error_flags.

  You should avoid using this for errors other than very serious ones where
  you cannot do anything until you get user feedback.  For other cases, use
  some sort of multi-tasking window so that all tasks do not stop functioning
  while you wait for a response.
*/


extern int Wimp_ReportErrorR(os_error *error, int flags, const char *name);
/*
  This is identical to Wimp_ReportError, except it passes on the return
  code from the SWI so you can detect which button has been pressed
  (1 for OK, 2 for Cancel).
*/


extern os_error *Wimp_SendMessage(event_type       eventtype,
                                  message_block    *message,
                                  message_destinee destinee,
                                  icon_handle      icon);
/*
  This function sends a message to a task or broadcasts one to all tasks.
  'type' contains the event code for the message you want to send,
  'msg' contains the message data and 'dest' contains the task handle or
  window handle of the destinee, or 0 to broadcast it.  If 'destinee' is the
  icon bar (-2), then 'icon' contains the icon number.

  This is almost always used to send user messages, for which it fills in
  the sender and the 'myref' fields on your behalf.  It can, however, be
  used to send any Wimp event.
*/


extern os_error *Wimp_AddMessages(int *messages);
/*
  This function extends the set of user messages the task is sent by the
  Wimp.  You pass a pointer to a zero-terminated array of user message codes,
  and the Wimp adds these to the messages given when you called
  Wimp_Initialise to allow you to receive these messages as well.
*/


extern os_error *Wimp_RemoveMessages(int *messages);
/*
  This funciton removes messages from the list of user messages the task is
  sent by the Wimp.  You pass a pointer to a zero-terminated array of user
  message codes, and the Wimp removes these from the messages your task is
  currently set to receive.
*/


/* window handling */

extern os_error *Wimp_OpenTemplate(const char *name);
/*
  This function opens the specified template file and reads in some header
  information about the file.  Only one template file may be open at a
  time.  You can then call Wimp_LoadTemplate to load each template and
  then Wimp_CloseTemplate to close the file.
*/


extern os_error *Wimp_CloseTemplate(void);
/*
  This function closes the template file previously opened with
  Wimp_OpenTemplate.
*/


extern os_error *Wimp_LoadTemplate(template_block *wtemplate);
/*
  This function loads a template.  You must previously have called
  Wimp_OpenTemplate to open the template file from which you want
  to load the template.
*/


extern os_error *Wimp_CreateWindow(window_block *block, window_handle *window);
/*
  This function tells the Wimp what the characteristics of a window are.
  The window definition is held in 'block' and the handle of the window
  is returned in 'window'.
*/


extern os_error *Wimp_OpenWindow(window_openblock *block);
/*
  This function updates the list of active windows (ones that should
  be displayed by the Wimp).
*/


extern os_error *Wimp_CloseWindow(window_handle window);
/*
  This function removes the specified window from the list of active
  window (ones that should be displayed by the Wimp).
*/


extern os_error *Wimp_DeleteWindow(window_handle window);
/*
  This functions closes the specified window if it is open, and then removes
  the definition of the window from memory.
*/


extern os_error *Wimp_RedrawWindow(window_redrawblock *block, BOOL *more);
/*
  This function is used to start a redraw of the parts of a window that
  are not up to date.  It should be called as the first Wimp operation after
  the Wimp Poll which returned the redraw window request.

  Wimp_RedrawWindow draws the window outline and then exits via
  Wimp_GetRectangle, which returns the coordinates of the first invalid
  rectangle of the work area.  It also returns a flag 'more' saying
  whether there is anything else to redraw.
*/


extern os_error *Wimp_UpdateWindow(window_redrawblock *block, BOOL *more);
/*
  This function is used to start a redraw of the parts of a window that
  are not up to date.  On entry, 'block' contains the area of the window
  to be updated.  The function exits via Wimp_GetRectangle which returns
  the coordinates of the first rectanlge of the work area.  It also
  returns a flag 'more' saying whether there is anything else to redraw.
*/


extern os_error *Wimp_GetRectangle(window_redrawblock *block, BOOL *more);
/*
  This function is used repeatedly following a call to Wimp_RedrawWindow or
  Wimp_UpdateWindow, until 'more' is returned as FALSE.  You should redraw
  the rectangles of the window's work area that are returned in 'block'.
*/


extern os_error *Wimp_GetWindowState(window_handle window, window_state *state);
/*
  This function finds the state of the given window, and returns it in
  'state'.
*/


extern os_error *Wimp_GetWindowInfo(window_info *info);
/*
  This returns information about the window specified by info->window.
  It returns a window_info block *and* the icon blocks for all the
  icons in the window.  This means that it will write past the end of
  the window_info block.  If you want to read the icons, allocate a
  block of memory you know to be big enough to hold them all and use
  this call.  Otherwise, you are probably better off using Window_GetInfo,
  which returns just the "window" part of the information.
*/


extern os_error *Wimp_GetWindowInfoNoIcons(window_info *info);
/*
  This returns information about the window specified by info->window.
  It returns only the window_info block, not the data about the icons
  in the window.
*/


extern os_error *Wimp_GetWindowOutline(window_outline *outline);
/*
  This function returns the given window's bounding box - the box which
  completely covers the window including all scrollbars, title bar, etc.
*/


extern os_error *Wimp_SetExtent(window_handle window, wimp_box *newextent);
/*
  This function sets the work area extent of the specified window to
  be the box specified in 'newextent'.
*/


extern os_error *Wimp_ForceRedraw(window_redrawblock *block);
/*
  This function marks an area of screen to be redrawn.  block->window should
  contain the window handle and block->rect then contains the area of the
  work area to redraw.  If block->window is -1, then block->rect contains
  a screen area to redraw rather than a window work area.
*/


/* Nested Wimp functions */

extern os_error *Wimp_OpenWindowNest(window_openblocknest *block,
                                     window_handle parent,
                                     window_nestflags flags);
/*
  This is a version of Wimp_OpenWindow for the nested Wimp, which allows
  access to the new features.
*/


extern os_error *Wimp_RedrawTitle(window_handle window);
/*
  This forces the redraw of the titlebar of the given window.

  This is only available under the Nested Wimp.
*/


extern os_error *Wimp_Enumerate(window_enumstack item, window_handle *window);
/*
  This function allows you to enumerate the windows in the window stack.
  You pass in a window handle (or -1 for the top level), and the type of
  information you want to find out, as defined by 'item'.  You can use
  this to find the window handle of the parent window, the top child window,
  the botom child window, the sibling immediately below or the sibling
  immediately above.

  The window handle you asked for is returned in 'window'.  If there is no
  matching window, -1 is returned in 'window.

  This is only available under the Nested Wimp.
*/


/* Icon handling */

extern os_error *Wimp_CreateIcon(icon_createblock *block, icon_handle *icon);
/*
  This function tells the Wimp to create an icon, whose characteristics are
  given in 'block'.  The handle of the created icon is returned in 'icon'.
*/


extern os_error *Wimp_DeleteIcon(window_handle window, icon_handle icon);
/*
  This function removes the definition of the specified icon from the
  gven window.  If it is not the last icon in the window, is is just marked
  as deleted.  If it is the last icon, the space occupied by it is freed.
  This call does not update the screen unles you are deleting an icon bar
  icon.  You should call Wimp_ForceRedraw to redraw the area yourself.
*/


extern os_error *Wimp_SetIconState(window_handle window,
                                   icon_handle   icon,
                                   unsigned int  value,
                                   unsigned int  mask);
/*
  This function sets the state of the given icon.  The value of a bit in
  the new flags is given as follows: if value and mask are both zero,
  the bit is left unchanged; if the mask is zero and the value one, the bit
  is toggled; if the mask is one and the value is the zero, the bit is
  cleared; and if the mask is one and the value is one the bit is set.

  ie. the new flags are given by (old flags AND NOT mask) EOR value
*/


extern os_error *Wimp_GetIconState(window_handle window,
                                   icon_handle   icon,
                                   icon_block    *iconinfo);
/*
  This function returns the specified icon's definition in the block pointed
  to by 'iconinfo'.
*/


extern os_error *Wimp_WhichIcon(window_handle    window,
                                   icon_handle   *icons,
                                   unsigned int   mask,
                                   unsigned int   settings);
/*
  This function searches for icons that match a given flag word.  All icons
  in the given window will be checks, and the list placed in the array pointed
  to be 'icons'.  'mask' determines which which bits are to be checked,
  'settings' says what should be looked for.

  It checks if: iconflags & mask == settings & mask.
*/


extern os_error *Wimp_PlotIcon(icon_block *fakeicon);
/*
  This function can be used to plot an icon in a window during a window
  redraw or update loop.  The icon doesn't exist as part of the window
  definition - you give the definition directly to this function to
  plot it.  If you call this outside a redraw loop the coordinates are
  screen coordinates rather than work area coordinates.
*/


/*  Poll handling */
/*  DeskLib 2.53:
 *  Old RISC OS 2 support has been removed.  DeskLib now always saves
 *  the saves of FP registers (because it's easy to ask the Wimp to).
 */

#define Wimp_Poll(mask, evt) Wimp_Poll3(mask, evt, 0)
/*
  Macro: os_error *Wimp_Poll(event_pollmask mask, event_pollblock *event);

  This function polls the Wimp to see whether certain events have occurred.
  You call it in the main loop of any program you wish to run under the Wimp.
  This lets the Wimp pass control to other taks, and it returns when there
  is some event for you to process.

  You pass a pollmask in 'mask' which determines which events you want to
  be notified about, and it returns the results of the poll in 'event'.
*/


#define Wimp_PollIdle(mask, evt, time) Wimp_PollIdle3(mask, evt, time, 0)
/*
  Macro: os_error *Wimp_PollIdle(event_pollmask *mask,
                                 event_pollblock *event, int earliest);

  This function polls the Wimp to see whether certain events have occurred.
  You call it in the main loop of a program you wish to run under the Wimp.
  This lets the Wimp pass control to other tasks, and it returns when there
  is some event for you to process.

  You pass a pollmask in 'mask' which determines which events you want to
  be notified about, and it returns the results of the poll in 'event'.

  You pass the earliest monotonic time you want a null event to be returned
  in 'earliest'.  You can use this when you want to do things in the
  background, to prevent your task from using more processor time than
  you need.
*/


extern os_error *Wimp_Poll3(event_pollmask mask, event_pollblock *event,
                            void *pollword);
/*
  This function polls the Wimp to see whether certain events have occurred.
  You call it in the main loop of a program you wish to run under the Wimp.
  This lets the Wimp pass control to other taks, and it returns when there
  is some event for you to process.

  You pass a pollmask in 'mask' which determines which events you want to
  be notified about, and it returns the results of the poll in 'event'.
  If you want to be notified of a pollword non-zero event, you should set
  the appropriate flag in the event mask, and pass a pointer to the
  pollword in 'pollword'.
*/


extern os_error *Wimp_PollIdle3(event_pollmask mask, event_pollblock *block,
                                int earliest, void *pollword);
/*
  This function polls the Wimp to see whether certain events have occurred.
  You call it in the main loop of a program you wish to run under the Wimp.
  This lets the Wimp pass control to other tasks, and it returns when there
  is some event for you to process.

  You pass a pollmask in 'mask' which determines which events you want to
  be notified about, and it returns the results of the poll in 'event'.If
  you want to be notified of a pollword non-zero event, you should set the
  appropriate flag in the event mask, and pass a pointer to the pollword
  in 'pollword'.

  You pass the earliest monotonic time you want a null event to be returned
  in 'earliest'.  You can use this when you want to do things in the
  background, to prevent your task from using more processor time than
  you need.
*/


/* Pointer/Mouse handling */

extern os_error *Wimp_GetPointerInfo(mouse_block *ptrinfo);
/*
  This function returns information about the position of the pointer
  and the instantaneous state of the mouse buttons, in the block
  pointed to by 'ptrinfo'.
*/


extern os_error *Wimp_SetPointerShape(pointer_shapeblock *shape);
/*
  This function sets the shape and active point of the pointer.
  This SWI is no longer the recommended way to do this - use
  Wimp_SpriteOp 36 instead.
*/


extern os_error *Wimp_DragBox(drag_block *draginfo);
/*
  This function initiates a Wimp drag operation.  You pass in the drag type,
  the initial size of the box being dragged or the window to be affected,
  and where approriate  a rectangle to specify a limit for the drag.
*/


/* Caret handling */

extern os_error *Wimp_SetCaretPosition(caret_block *caret);
/*
  This function removes the caret from its old position and redraws it in a
  new position, as defined by 'caret'.  If caret->window is -1, the caret
  is removed from its current location.

  If caret->height is -1 then the offset and height are calculated from the
  given window, icon and index.  If caret->index is -1 then the index and
  height are calculated from the given window, icon and offset.
*/


extern os_error *Wimp_GetCaretPosition(caret_block *caret);
/*
  THis function returns details of the caret's state in the block pointed
  to by 'caret'.  If the caret is not currently in any window, caret->height
  is returned as -1.
*/


/* Menu handling */

extern os_error *Wimp_CreateMenu(menu_ptr menu, int xpos, int ypos);
/*
  This function opens a Wimp menu from the details supplied in 'menu'.
  The menu is initially displayed with its top-left corner at the screen
  coordinated (xpos, ypos).
*/


extern os_error *Wimp_CreateSubMenu(menu_ptr menu, int xpos, int ypos);
/*
  This function is called to open a submenu when you receive a MenuWarning
  message from the Wimp.  The three arguments can be obtained from the
  message, or altered by you if required.
*/


extern os_error *Wimp_DecodeMenu(menu_ptr menu, int *selections, char *result);
/*
  This converts the given list of menu selections for the given menu to
  a text string made up of the text in the selected menu items.
  eg. "Options.Confirm" for a filer menu.

  Note that this is a CR-terminated string, and *not* a nul-terminated
  C string.  You should also make sure that the buffer pointed to by
  'result' is big enough to hold the resulting string.
*/


/* Keyboard handling */

extern os_error *Wimp_ProcessKey(int character);
/*
  This call causes the wimp to offer the given character to other applications.
  This can be used to pass on keypress events your program is not interested
  in, or to insert "new" keypresses into the wimp buffer.

  You should not insert more that one or two characters at a time this way,
  or you will confuse the Wimp.  Characters inserted this way take precendence
  over characters actually typed.
*/


/* Graphics/Screen handling */

extern os_error *Wimp_SetMode(int mode);
/*
  This function changes the Wimp mode to that given.  You should generally
  avoid its use, and make your program work in any mode.
*/


extern os_error *Wimp_SetColour(int colour);
/*
  This function sets the current graphics colour to one of the sixteen
  standard Wimp colours, and the GCOL plot action.  For ease of use, you
  should probably cast the argument from a wimp_colour.
*/


extern os_error *Wimp_ReadPalette(palette_block *palette);
/*
  This function reads the current Wimp palette settings into the
  block pointed to by 'palette'.
*/


extern os_error *Wimp_SetPalette(palette_block *palette);
/*
  This function sets the Wimp palette to that given in 'palette'.
  In sixteen-colour modes the video palette is also reprogrammed, and in
  other modes the colou lookup table may change.

  You should force a redraw of the screen after calling this function.
*/


extern os_error *Wimp_ReadPixTrans(unsigned int areaindex, void *area,
                                   void *ptsprite, scale_block *scaleblock,
                                    char *transblock);
/*
  This function finds out how the Wimp would plot the given sprite.
  If areaindex is less than 256 the sprite is in a system area, if
  it is less that 512 it is in the given user area and 'ptsprite'
  points to the sprite name and if it is less than 768 then 'ptsprite'
  points directly to the sprite.  'area' points to the user sprite area,
  1 for the Wimp area or zero for the system sprites area.

  On exit, 'scaleblock' and 'transblock' will contain information on how
  the Wimp would display the sprite.
*/


extern os_error *Wimp_SetFontColours(int foreground, int background);
/*
  This function sets the anti-aliased font colours based on the foreground
  and background Wimp colours given.  The colours will not be preserved
  across calls to Wimp_Poll.
*/


extern os_error *Wimp_SpriteOp(int r[10]);
/*
  This function allows operations on Wimp sprites without having to
  specify the sprite area pointer.  r[0] contains the reason code,
  r[2] contains a pointer to the sprite nameand r[3] onwards contain the
  other parameters.  Only read operations are allowed with this function.
*/
/*
  This used to be:
  extern os_error *Wimp_SpriteOp(_kernel_swi_regs *registers);
*/


extern os_error *Wimp_BaseOfSprites(void **rom_base, void **ram_base);
/*
  This function returns the addresses of the ROM and RAM parts of the
  Wimp sprite pool.  The RAM address moves around as its contents change.
  You should not normally need to know these locations.
*/


extern os_error *Wimp_BlockCopy(window_handle window, wimp_box *source,
                                int x, int y);
/*
  This function copies as much as possible of the source rectangle to
  another position (x, y).  Any part that cannot be copied will be
  invalidated by the Wimp, to be redrawn by you.  This call cannot be made
  from inside a redraw or update loop.
*/


extern os_error *Wimp_CommandWindow(int x);
/*
  This function opens a text window in which VDU 4-type output can be displayed.
  The exact action depends on the value of 'x'.

  If x is greater than 1, it is a pointer to the window title - the window is
  only opened the next time a character is printed on screen.  If x is 1, the
  window is set to "active".  If x is zero then window is closed after giving
  a "Press SPACE or click mouse to continue".  If x is -1 the window is closed
  without giving a prompt.
*/


#ifdef __cplusplus
}
#endif


#endif
