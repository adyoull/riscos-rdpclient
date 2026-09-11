/*
    ####             #    #     # #
    #   #            #    #       #          The FreeWare C library for
    #   #  ##   ###  #  # #     # ###             RISC OS machines
    #   # #  # #     # #  #     # #  #   ___________________________________
    #   # ####  ###  ##   #     # #  #
    #   # #        # # #  #     # #  #    Please refer to the accompanying
    ####   ### ####  #  # ##### # ###    documentation for conditions of use
    ________________________________________________________________________

    File:    Icon.h
    Author:  Copyright © 1992, 1993, 1994 Jason Williams
    Version: 1.15 (17 Apr 2005)
    Purpose: High-level icon handling routines
*/


#ifndef __dl_icon_h
#define __dl_icon_h

#ifndef __dl_wimp_h
#include "DeskLib:Wimp.h"
#endif

#ifndef __dl_dragaspr_h
#include "DeskLib:DragASpr.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif


/* Abstract */
/*
  This header defines some relatively high-level routines for handling icons.
*/

extern icon_handle Icon_BarIcon(const char *spritename, window_handle pos);
/*
  This creates a sprite-only iconbar icon with the normal default flags
  for such an icon.  If you want something more complicated you should
  create it yourself.

  If you want to attach handlers or otherwise make use of the icon,
  you should store the returned icon handle for use with Event_Claim or
  similar. The returned icon handle is 0 if there is an error.

  You pass the name of a sprite in the Wimp sprite pool and 'pos' which
  should be iconbar_LEFT or iconbar_RIGHT.  The icon will be created the
  correct size for the named sprite.
*/


extern icon_handle Icon_BarIconUser(const char *spritename, window_handle pos,
                                    void *area);
/*
  This creates a indirected sprite-only iconbar icon from a sprite in a user
  sprite area.

  If you want to attach handlers or otherwise make use of the icon,
  you should store the returned icon handle for use with Event_Claim or
  similar.

  You pass the name of a sprite, a pointer to the sprite area in which it
  is stored and 'pos' which should be iconbar_LEFT or iconbar_RIGHT.
  The icon will be created the correct size for the named sprite.
*/


extern BOOL Icon_GetSelect(window_handle window, icon_handle icon);
/*
  This returns TRUE if the given icon is selected, and FALSE otherwise.
*/


extern BOOL Icon_GetShade(window_handle window, icon_handle icon);
/*
  This returns TRUE if the given icon is currently shaded (greyed out).
*/


extern void Icon_SetSelect(window_handle window, icon_handle icon, int flag);
/*
  This sets whether or not the given icon is selected. If flag is 0 it
  means will be deselected, 1 means it will be selected.
*/

extern void Icon_SetDeleted(window_handle window, icon_handle icon, int flag);
/*
  This sets whether or not the given icon has its "deleted" flag set. If the
  value of flag passed to the function is 0, the "deleted" icon flag will be
  unset. If 1 is passed, the icon "deleted" flag will be set.
*/


extern void Icon_Select(window_handle window, icon_handle icon);
/*
  If the given icon is currently deselected, it is selected.
*/


extern void Icon_Deselect(window_handle window, icon_handle icon);
/*
  If the given icon is currently selected, it is deselected.
*/


extern void Icon_SetShade(window_handle window, icon_handle icon, int flag);
/*
  This sets whether or not the given icon is shaded. If flag is 0 it
  means will be unshaded, 1 means it will be shaded.
*/


extern void Icon_Shade(window_handle window, icon_handle icon);
/*
  If the icon is currently unshaded (active) it is shaded (made inactive)
  This includes removing the caret from the icon.
*/


extern void Icon_Unshade(window_handle window, icon_handle icon);
/*
  If the icon is currently shaded (inactive) it is unshaded (made active).
*/


extern void Icon_SetForeColour(window_handle window, icon_handle icon,
                               int wimpcolour);
/*
  This sets an icon's foreground colour to the given Wimp colour (0-15).
  This shouldn't be used on icons with the "outline font" flag set.
*/


extern void Icon_SetBackColour(window_handle window, icon_handle icon,
                               int wimpcolour);
/*
  This sets an icon's background colour to the given Wimp colour (0-15).
  This shouldn't be used on icons with the "outline font" flag set.
*/


extern void Icon_ForceWindowRedraw(window_handle window, icon_handle icon);
/*
  This forces a redraw of a small area of the window around and including
  the given icon.

  You will probably have no use for this now that 3D borders are supplied
  by the Wimp rather than external modules.
*/


#define Icon_ForceRedraw(wind, icon) Wimp_SetIconState(wind, icon, 0, 0);
/*
  MACRO: os_error *Icon_ForceRedraw(window_handle wind, icon_handle icon);

  This gets the Wimp to redraw the given icon.  It returns NULL if there
  is no error.
*/


extern void Icon_SetCaret(window_handle window, icon_handle icon);
/*
  The places the caret in the given icon, and the righthand side of any
  text in the icon.  If it isn't a text icon, nothing will happen.
*/


extern void Icon_LoseCaret(window_handle window, icon_handle icon);
/*
  This ensures that the caret will not be in the given icon when it returns.
  If the caret is not in the icon it does nothing.
*/


extern void Icon_SetInteger(window_handle w, icon_handle i, int value);
/*
  This sets the text in the given icon to the decimal representation
  of 'value', and redraw the icon.  If the number has too many digits
  it will be truncated to fit.  If it cannot set the text it fails quietly.
*/


extern void Icon_SetDouble(window_handle w, icon_handle i,
                           double value, int decimalplaces);
/*
  This sets the text in the given icon to the decimal representation of
  'value' and redraws the icon.  Onlt the first 'decimalplaces' digits
  after the decimal point are printed.  If the number has too many digits
  it will be truncated to fit.  If it cannot set the text is fails quietly.
*/


extern void Icon_SetText(window_handle w, icon_handle i, const char *text);
/*
  This copies 'text' into the icon's indirected text buffer and redraws the
  icon.  If the text won't fit in the buffer, it is truncated.  If 'text' is
  NULL, it places " " in the icon.  If it cannot set the text it fails quietly.
*/


extern void Icon_SetTextRJ(window_handle w, icon_handle i, const char *text);
/*
  This copies 'text' into the icon's indirected text buffer and redraws the
  icon.  This text is "right-justified", so if the text won't fit in the
  buffer, it is truncated at the *left* hand side preceded by an ellipsis
  character ('...').

  If 'text' is NULL, it sets the icon text to '\0'.  If it cannot set the
  text it fails quietly.
*/


extern void Icon_printf(window_handle window, icon_handle icon,
                        const char *format, ...);
/*
  This "prints" text to the given icon under the control of the
  printf-style format string 'format'.  This will have no effect on
  any icon that isn't indirected.

  Note that while text that is too long for the indirected text buffer will
  be truncated, if the resulting string before truncation is longer than
  512 characters nastiness will occur (ie. your program will crash).
*/


extern double Icon_GetDouble(window_handle w, icon_handle i);
/*
  Gets the given icon's text and returns it in the form of a "double"
  floating-point value. 0 will be returned from any error/invalid text.
*/

extern int Icon_GetInteger(window_handle w, icon_handle i);
/*
  Gets the given icon's text and returns it in the form of an integer
  numeric value. 0 will be returned from any error/invalid text
*/


extern void Icon_GetText(window_handle w, icon_handle i, char *text);
/*
  Copies the text string from the icon (sprite name, text, or indirected)
  into the string pointed to by 'text'.

  Note that 'text' must be big enough to hold the copied string, so the
  minimum sizes are 12 characters for a non-indirected icon and the size of
  the icon's buffer for an indirected icon.
*/


extern char *Icon_GetTextPtr(window_handle w, icon_handle i);
/*
  This returns a pointer to the actual indirected text buffer for the
  given icon.  If the icon is not indirected it returns NULL.
*/


#define Icon_SetFlags(window, icon, flags, set) \
  Wimp_SetIconState((window), (icon), ((set) ? (flags) : 0), (flags))
/*
  MACRO: os_error *Icon_SetFlags(window_handle w, icon_handle i,
                                 int flags, BOOL set);

  This is a neat interface to the Wimp_SetIconState SWI for the given
  icon.  If 'set' is TRUE, all the flags given in 'flags' will be set to 1,
  otherwise they will be set to 0.

  It returns NULL if there is no error.
*/


#define Icon_SetFgCol(w, i, col) \
Wimp_SetIconState((w), (i), (col) * icon_FORECOLOUR, 0xF * icon_FORECOLOUR)
/*
  MACRO: os_error *Icon_SetFgCol(window_handle window, icon_handle icon,
                                 int col);

  This is a macro to set the foreground colour of an icon.
  This shouldn't be used on icons with the "outline font" flag set.
*/


#define Icon_SetBgCol(w, i, col) \
Wimp_SetIconState((w), (i), \
                (unsigned) (col) * (unsigned) icon_BACKCOLOUR, \
                (unsigned) 0xF * (unsigned) icon_BACKCOLOUR)
/*
  MACRO: os_error *Icon_SetBgCol(window_handle window, icon_handle icon,
                                 int col);

  This is a macro to set the background colour of an icon.
  This shouldn't be used on icons with the "outline font" flag set.
*/


extern void Icon_ShadeGroup(window_handle window,
                            icon_handle icons[],
                            BOOL shade);
/*
  This takes a window and a -1 terminated list of icon handles which
  specifies the icons to set the state of.  If 'shade' is TRUE the icons
  will all be shaded (greyed out), if it is FALSE they will be unshaded.
*/


extern void Icon_SelectGroup(window_handle window,
                             icon_handle icons[],
                             BOOL select);
/*
  This takes a window and a -1 terminated list of icon handles which
  specifies the icons to set the state of.  If 'select' is TRUE the icons
  will all be selected, if it is FALSE they will be deselected.
*/


extern int Icon_GetFgCol(icon_block *icon);
/*
  This takes an icon_block as returned by Wimp_GetIconState, and returns the
  foreground colour.  This copes with anti-aliased icons as well as "normal"
  ones.
*/


extern int Icon_GetBgCol(icon_block *icon);
/*
  This takes an icon_block as returned by Wimp_GetIconState, and returns the
  background colour.  This copes with anti-aliased icons as well as "normal"
  ones.
*/


extern void Icon_SetRadios(window_handle window,
                           icon_handle first, icon_handle last,
                           icon_handle onradio);
/*
  This sets one icon in a group, and unsets all the others.  The group is
  from the lowest-numbered icon ('first') to highest-numbered icon ('last')
  inclusive.
*/


extern int Icon_WhichRadio(window_handle window,
                           icon_handle first, icon_handle last);
/*
  This finds which icon in a group is selected.  The group is from the
  lowest-numbered icon ('first') to highest-numbered icon ('last') inclusive.
  It returns the icon handle of the first selected icon in the group, or
  'last' if none are selected.
*/


extern int Icon_WhichRadioInEsg(window_handle wh, int esg);
/*
  This finds which icon in the given ESG is selected.  This means the
  icons do not have to be consecutively numbered.

  It returns the icon handle of the selected icon.
*/


extern void Icon_ClickWait(int waittime);
/*
  This waits for the given number of centiseconds.

  This is a MULTITASKING wait - although your program will do nothing,
  the rest of the desktop will continue to run.
  The wait is aborted after the given time, or as soon as an event is
  received. If an attempt is made to re-enter ClickWait(), it will return
  immediately (i.e. if a click event causes ClickWait to ba called, then
  a second click while "waiting" from the first click will have no effect.
*/


extern void Icon_StartDrag(window_handle window, icon_handle icon);
/*
  Starts a drag operation based on the given icon.
  This is mainly used in "save as" windows to start drags of the file icon
  when it is clicked on, but may be useful elsewhere.

  This call DOES NOT use DragASprite.
*/


#define Icon_StartSolidDrag(w, i)  DragASprite_DragIcon(w, i)
/*
  MACRO: os_error *Icon_StartSolidDrag(window_handle w, icon_handle i);

  Starts a drag operation based on the given icon, using DragASpr if available.
*/


extern void Icon_DisposeIndData(icon_data *data, icon_flags flags);
/*
  This will free any indirected data for this icon. It uses free(), so
  obviously expects that malloc() was used to get the memory in the first
  place, so make sure this is the case if you use this function.
*/


extern icon_handle Icon_FileIcon(window_handle window, icon_handle icon,
                                 int filetype);
/*
  This changes the given icon so that it contains the correct filetype sprite
  for the given filetype number (eg. file_ff9 for filetype 0xff9).

  This requires that the icon is already an indirected icon with enough
  room in the buffer to hold the sprite name.  This function converts
  it to an indirected sprite-only icon, fills in the sprite name and sets
  the sprite area to the Wimp pool.  It doesn't check that the sprite
  actually exists.

  It returns the new icon handle, in case it has changed when it was
  recreated.
*/


extern void Icon_ScreenPos(window_handle window,
                           icon_handle icon,
                           wimp_rect *rect);
/*
  This finds the screen coordinates of the given icon, and places them
  in 'rect'.
*/


extern int Icon_SetSlider(window_handle window,
                          icon_handle baseicon, icon_handle slidericon,
                          int sliderpos);
/*
  This sets a slider icon-pair to the specified position, which is an
  integer value between 0 and 1000 (a percentage * 10, if you like).
  Out-of-range values are clipped to be 0 or 1000, depending.  This
  function returns the value the slider has been set to.

  A slider pair consists of two icons: a "base" icon which can provide a
  border if you wish, and provides the boundary for the "slider" icon;
  and a "slider" icon which is a left-end-fixed, right-end-draggable bar
  graph type thing, like in the Task display.

  Values are read and set for the slider in the form of in integer
  representing the width of the slider as thousandths of the width of base.
  When the slider is as wide as the base, the value is 1000; when it is at
  minimum size the value is zero.

  The base icon should have a lower icon handle than the slider so that the
  slider appears on top of the base.  When either icon is clicked you
  initiate the drag with Icon_DragSlider and then continuously update the
  position by calling Icon_UpdateSlider on NULL (or click if buttontype is
  set to "Always") events.

  All Icon_Slider calls return the current slider position so that you may
  update things which depend on the value (eg. a colour display when a
  "red" slider is dragged.

  Note that the slider is "moved" by deleting and recreating the slider icon,
  so can go wrong if other icons have been deleted in the window.
*/


extern int Icon_UpdateSlider(window_handle window,
                             icon_handle baseicon, icon_handle slidericon,
                             int lastpos);
/*
  You call this with a slider icon-pair (see Icon_SetSlider for details),
  and the last known position of the slider.  It works out the position
  which corresponds with the current pointer position and updates it if
  it is out of date.  You do this regularly while the icon is being dragged,
  to keep the slider in sync with the drag.

  This returns the updated position in thousandths of the maximum width
  (a percentage * 10, if you will).

  Note that the slider is "moved" by deleting and recreating the slider icon,
  so can go wrong if other icons have been deleted in the window.
*/


extern int Icon_DragSlider(window_handle window,
                           icon_handle baseicon, icon_handle slidericon);
/*
  You call this with a slider icon-pair (see Icon_SetSlider for details),
  when either the slider icon or the base icon is clicked on.
  It works out the position which corresponds with the current pointer
  position and updates it if it is out of date.  You should then call
  Icon_UpdateSlider regularly for as long as the drag continues.

  This returns the updated position in thousandths of the maximum width
  (a percentage * 10, if you will).  You should remember this so that
  you can pass it to Icon_UpdateSlider as the last known position.

  Note that the slider is "moved" by deleting and recreating the slider icon,
  so can go wrong if other icons have been deleted in the window.
*/


extern int Icon_ReadSlider(window_handle window,
                           icon_handle baseicon, icon_handle slidericon);
/*
  You call this with a slider icon-pair (see Icon_SetSlider for details),
  and it returns the position of the slider in thousandths of the maximum
  width (a percentage * 10, if you will)
*/


typedef struct
{
  window_handle window;
  icon_handle   texticon;
  icon_handle   incrementicon;
  icon_handle   decrementicon;
  BOOL          loop;
  unsigned int  step;
  int           min;
  int           max;
} icon_incdecblock;
/*
  This type holds data for the IncDec handlers.  Most of the fields are
  self-explanatory.  If' loop' is TRUE then the value in 'texticon' will
  looop around at min and max limits rather than stopping.  The step is
  the step to be used for both incrementing and decrementing the value.
*/


extern icon_incdecblock *Icon_InitIncDecHandler(
                                           const window_handle window,
                                           const icon_handle   texticon,
                                           const icon_handle   incrementicon,
                                           const icon_handle   decrementicon,
                                           const BOOL          loop,
                                           const unsigned int  step,
                                           const int           min,
                                           const int           max,
                                           const int           start);
/*
  This sets up an IncDec icon set.  This is a set of three icons: an
  indirected display icon 'texticon', and two icons to "bump up" and
  "bump down" the integer value in that icon.  You also specify an upper
  and lower limit for the integer in 'min' and 'max' and can choose
  whether you want the values to loop around when you go past the limits
  (by setting loop to TRUE) or just stop.  'step' is used as the step for
  both incrementing and decrementing the value.  'start' is set as the
  initial value in the 'texticon'.

  This uses Event_Claim to claim clicks on the increment and decrement icons
  (so you must have initialised and be using the Event system) and handles
  Select and Adjust clicks on those icons correctly.

  It returns NULL if it couldn't claim the events, or else a pointer to
  the block of data about this icon set, which you will need to release
  the events with Icon_ReleaseIncDecHandler.
*/


extern BOOL Icon_ReleaseIncDecHandler(icon_incdecblock *incdecblock);
/*
  This releases the handlers set up by Icon_InitIncDecHandler and frees
  the memory used by the icon_incdecblock for this icon set.  It returns
  TRUE if the handlers were released successfully, and FALSE otherwise.
*/


extern void Icon_AlterValidation(window_handle window, icon_handle icon,
                                 const char *newvalidation);
/*
  This alters the validation string of the given icon to hold the text
  given in 'newvalidation', and redraws the icon.  If the validation
  string is already set to the given value, it does nothing.

  The buffer for the validation string *must* be big enough to contain
  the new value, or you will overwrite and corrupt other pieces of
  memory.
*/


extern BOOL Icon_ButtonIsHeld(void);
/*
  This returns TRUE if any mouse button is held down.
*/


#ifdef __cplusplus
}
#endif


#endif
