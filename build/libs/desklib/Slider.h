/*
    ####             #    #     # #
    #   #            #    #       #          The FreeWare C library for
    #   #  ##   ###  #  # #     # ###             RISC OS machines
    #   # #  # #     # #  #     # #  #   ___________________________________
    #   # ####  ###  ##   #     # #  #
    #   # #        # # #  #     # #  #    Please refer to the accompanying
    ####   ### ####  #  # ##### # ###    documentation for conditions of use
    ________________________________________________________________________

    File:    Slider.h
    Author:  Copyright © 1994 Peter Gaunt
    Version: 1.01 (17 Apr 2005)
    Purpose: Provide a clean way of using 'slider' icons.  These routines
             are more general-purpose, friendly and efficient that those
             found in the Icon library.  (The Icon slider functions are left
             in for backwards compatibility at the moment - you should
             use these functions in preference).
*/

#ifndef __dl_slider_h
#define __dl_slider_h

#ifndef __dl_core_h
#include "DeskLib:Core.h"
#endif

#ifndef __dl_wimpswis_h
#include "DeskLib:WimpSWIs.h"
#endif

#ifndef __dl_sprite_h
#include "DeskLib:Sprite.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif


/* Abstract */
/*
  This header provides functions for dealing with slider icons.  They are
  designed to be much more efficient than the slider functions in the Icon
  library.  It user sliders redrawn in a redraw loop rather than actually
  resizing the icons every time and therefore requires windows using these
  routines to be have their auto-redraw flag clear.

  Sliders need a base icon, which should be empty (ie. no text or sprite set
  for it) with an unfilled background.  The window should have its auto-redraw
  flag clear.  To use a slider, you need to call Slider_Redraw in the redraw
  loop and call Slider_Drag when the slider icon is clicked on.

  The boundary for the slider icon should not overlap any part of the window
  which is user-redrawn, as you will get messy effects.

  Note that the Slider functions assume that the variables set by
  Screen_CacheModeInfo are kept up to date, so you should make provision for
  it to be called on a mode change (eg. through Handler_ModeChange).
*/


#define SLIDER_MAX 100000
/*
  This is the top end of the internal representation of a slider position.
  The range is 0..SLIDER_MAX.  The values which are returned to you will be
  in whatever units you set.
*/

typedef struct slider_info slider_info;

struct slider_info{
  window_handle window;
  icon_handle   icon;

  int value;

  struct
  {
    int         min;
    int         max;
  } limits;

  struct
  {
    int foreground;
    int background;
  } colour;

  struct
  {
    int x;
    int y;
  } border;

  struct
  {
    sprite_areainfo *spritearea;
    sprite_header   *sprite;
  } knob;

  struct
  {
    int  vertical  :1;
    int  rgb       :1;
    int  dragging  :1;
    int  clickstop :1;
    int  reserved  :28;
  } flags;

  int (*update)(slider_info *slider, void *ref);
  int reference;
};
/*
  This structure is used to define a slider icon.  You set it up "by hand"
  to define the slider and then pass it to the various Slider functions to
  redraw, set the value, etc.

  'window' and 'icon' should be fairly obvious.  The icon specified should
  be transparent (unfilled) and respond to clicks if you want to be able to
  drag rather than just using it for display - you can then call Slider_Drag
  when the icon is clicked.

  'limits' define the values returned when the slider is at its minimum or
  maximum extent.  The other slider positions will return linearly spaced
  intermediate values.  The slider will not be set to less than limits.min
  or more than limits.max.

  'colours' contains the foreground and background colours for the slider,
  and should be set to -1 if you want an invisible slider.  If flags.rgb
  is unset then the colour values are logical Wimp colours.  If it is set
  they are RGB values suitable for passing to ColourTrans (ie. 0cBBGGRR00
  format).

  'border' specifies the size of blank border which will be left between
  the slider and the base icon in OS units.  If the base icon has a border
  then setting the 'border' values to zero will cause them to flicker when
  the slider is redrawn.  A border of 4 on all sides should avoid this problem
  with most icon borders and a value of 12 gives sliders similar to those
  suggested by the Style Guide if the icon has an "r2" recessed border.

  'knob' is not currently used but may be used in future to specify a
  sprite to be used as a knob.  For noe knob.spritearea should be set to
  NULL to indicate that there is no sprite supplied.

  'flags' contains the flags for the slider.  flags.vertical indicates that
  the slider is vertical rather than horizontal.  flags.rgb is described
  above (under the description of 'colours').  flags.dragging is for internal
  use, and should be set to zero, as should flags.reserved.  flags.clickstop
  means that the slider will jump between exact user values rather than being
  dragged as smoothly as possible.  The slider will only be redrawn when the
  user value changes eg. if min is 0 and max is 2, the slider will only have
  three states.

  'update' should either be NULL or set to point to a function which will be
  called if the slider value changes during calls to Slider_SetValue or
  Slider_Drag.  This allows you to update a text icon to match the current
  value, or otherwise keep track of the value as it changes rather than
  waiting until a drag has finished.

  The function should take two arguments, the first being a pointer to the
  slider_info block of the slider being updated, and the second being a
  reference specified when you call Slider_Drag or Slider_SetValue.  This
  can be any value you wish - it is typically used to point to some useful
  information to indicate what action is being performed, etc.

  The function should return non-NULL if it wants the drag function to
  stop dragging.  This allows you to stop the slider going above or
  below a maximum or minimum value independently of the values of
  limits.min and limits.max.

  'reference' can be used for any purpose you like.  If you set up more
  than one slider, it can be useful to give them each separate references
  so you can distinguish between them without having to check the window
  and icon handles.
*/


extern os_error *Slider_Redraw(slider_info *slider, wimp_rect *clipwindow);
/*
  You call this inside your redraw loop to keep the slider up to date.

  Simply pass the slider_info block for the slider you wish to redraw, and
  the clip rectangle you get back from Wimp_RedrawWindow or Wimp_GetRectangle
  in your redraw loop (this will be in screen coordinates).  It only does
  anything if part of the given slider is inside the supplied clipwindow.

  It returns NULL if successful, or a pointer to an error block if it cannot
  use the specified colour to draw the slider.
*/


extern int Slider_ReadValue(slider_info *slider);
/*
  You pass the slider_info block for the slider you are interested in, and
  it returns the current user slider value (ie. in the range you specified
  in that block).
*/


extern os_error *Slider_SetValue(slider_info *slider,
                                 int value,
                                 int *valueset,
                                 void *ref);
/*
  This sets the slider value (in user units), ensuring that the value is
  within the limits specified in the slider info.  If it is called while
  the slider is being dragged it does not set a new value.

  You pass in the slider_info block for the slider whose value you wish to
  set, and the value you wish to set it to.

  You can pass a pointer to an int in 'valueset' to receive the actual value
  of the slider on return from the function - this may not be the same as
  'value' if, for instance, 'value' is outside the allowed bounds for the
  slider.  Otherwise 'valueset' should be NULL.

  'ref' is any reference of your choice, which will be passed to the
  slider's update function if this call changes the value of the slider.

  This function returns NULL if successful, or a pointer to an error block
  if there is a problem accessing or redrawing the icon.
*/


extern os_error *Slider_Drag(slider_info *slider,
                             int *closed,
                             int *value,
                             void *ref);
/*
  This initialises (and handles) a drag on the given slider.  You should
  call this when the slider is clicked on.  It polls the Wimp, grabbing
  NULL events but passing others on to Event_Process, updating the slider
  position and calling the slider's update function as the value changes.

  This function returns when the user stops dragging,the slider's
  update function (if there is one) returns a non NULL value, or if
  the window containing the slider is closed.

  If 'closed' is not NULL, then it is set to TRUE if the window is closed
  during the drag.  If 'value' is not NULL, it is filled with the current
  value of the slider.

  This function returns NULL if successful, or a pointer to an error block
  if there is a problem accessing or redrawing the icon.
*/


#ifdef __cplusplus
}
#endif


#endif
