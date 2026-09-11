/*
    ####             #    #     # #
    #   #            #    #       #          The FreeWare C library for
    #   #  ##   ###  #  # #     # ###             RISC OS machines
    #   # #  # #     # #  #     # #  #   ___________________________________
    #   # ####  ###  ##   #     # #  #
    #   # #        # # #  #     # #  #    Please refer to the accompanying
    ####   ### ####  #  # ##### # ###    documentation for conditions of use
    ________________________________________________________________________

    File:    Sprite.h
    Author:  Copyright (c) 1992, 1993, 1994 John Winters, Jason Williams,
                                          Jason Howat
             and sundry contributors
    Version: 0.40 (28 Nov 2005)
    Purpose: Sprite-SWI interfaces.
             This version of the Sprite library only includes the most common
             sprite operations.

    Mods:    02 Sep 1995 JH Sprite_MemorySize now works in 'double pixel'
                            modes (see 'Libraries.Sprite.c.MemSize' for the
                            details).
                            Added Sprite_Select.
*/

#ifndef __dl_sprite_h
#define __dl_sprite_h

#include "DeskLib:Core.h"
#include "DeskLib:Wimp.h"
#include "DeskLib:Coord.h"
#include "DeskLib:Screen.h"

#include <stdarg.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Abstract */
/*
  This file provides interfaces to some of the Sprite-manipulation routines
  available on RISC OS.  This only includes operations on "named" sprites
  (rather than sprite pointers) in user sprite areas.
*/


#define sprite_MAXNAME    12
/* This is the maximum length of a sprite name. */


#define spritepool_SYSTEM 0
/*
  This is used to identify the system sprite pool in Sprite operations.
  You should never, ever have to use the system sprite area any more.
*/

#define spritepool_WIMP   1
/*
  This is used to identify the wimp sprite pool in Sprite operations.
*/


typedef struct
{
    unsigned int areasize;
    unsigned int numsprites;
    unsigned int firstoffset;
    unsigned int freeoffset;
} sprite_areainfo;
/*
  This is the control block for a sprite area - it comes at the start of the
  area and is followed in memory by an optional extension area (usually not
  used), sprites, and free space.

  The 'areasize' is the size of the entire sprite area, including this header.
  'numsprites' is the number of sprites currently in the area, 'firstoffset'
  is the offset to the first sprite and 'freeoffset' is the offset to the
  first free space from the begninning of the area.

  To initialise a new area you set the areasize correctly, set numsprites to
  zero and firstoffset and freeoffset to 16, then call Sprite_InitArea.
*/


typedef sprite_areainfo *sprite_area;
/*
  A sprite_area is a pointer to a spritearea - and area of memory set up to
  hold sprites.  It is defined as a pointer to the beginning of that area,
  where there should be a sprite_areainfo block.
*/


typedef struct
{
    unsigned int width;
    unsigned int height;
    unsigned int maskstatus;
    screen_modeval mode;
} sprite_info;
/*
  This holds information about a sprite, giving the width and height in
  pixels, whether or not it has a mask and the screen mode, or sprite type
  for a "new-format" sprite.
*/


typedef struct
{
    int          callno;
    sprite_area  spritearea;
    void         *sprite; /* Either const char *name, either const
                             sprite_header *sprite, depending on
                             callno & (1<<9) */
    void         *savearea;
} sprite_outputstate;
/*
  This is used to store the VDU state so that when you redirect VDU output
  to a sprite you can restore it to its previous state when you are done.
  You shouldn't have to access this directly, just use Sprite_Redirect and
  Sprite_UnRedirect.
*/


typedef struct
{
  int  offset_next;
  char name[sprite_MAXNAME];
  int  width;
  int  height;
  int  leftbit;
  int  rightbit;
  int  imageoffset;
  int  maskoffset;
  screen_modeval screenmode;
} sprite_header;
/*
  This is a structure to store a sprite header, the part of the sprite
  before this image data itself which gives the offset to the next sprite
  in the block, the name, and so on.  You will probably not have to access
  this directly, and if you do you can find full details about the
  structure and layout of a sprite area in the PRMs.
*/


typedef struct
{
  wimp_point multiply;
  wimp_point divide;
} sprite_scalefactors;
/*
  This holds scale factors for scaling a sprite when plotting it.
  It simply holds the multipliers and divisors for the x and y scale factors.
*/


extern os_error *Sprite_ScreenSave(const char *filename, BOOL savepalette);
/*
  This is a veneer for OS_SpriteOp 2.

  It saves the current graphics window (normally the whole screen) with the
  given filename.  If savepalette is TRUE, is saves the palette as well,
  FALSE means no palette is saved.  The name of the sprite in the file
  is "screendump".
*/


extern os_error *Sprite_ScreenLoad(const char *filename);
/*
  This is a veneer for OS_SpriteOp 3.

  This loads the first sprite in the given file and displays it at the bottom
  left of the screen.  It changes the mode and palette if necessary to display
  the sprite correctly.
*/


extern os_error *Sprite_InitArea(sprite_area area);
/*
  This is a veneer for OS_SpriteOp 9.

  It initialises the given sprite area to make it ready to use.
  You should set the area size and the offset to the first sprite
  (which is 16 if you are not using an extension area).
*/


extern os_error *Sprite_Load(sprite_area area, const char *filename);
/*
  This is a veneer for OS_SpriteOp 10.

  It loads all the sprites in the given file into the given sprite area.
  Any sprites already in the area are deleted.  The sprite area should
  have been initialised (eg. with Sprite_InitArea) before calling this,
  and be of sufficient size to hold the sprite file.
*/


extern os_error *Sprite_Merge(sprite_area area, const char *filename);
/*
  This is a veneer for OS_SpriteOp 11.

  It loads all the sprites in the given file into the given sprite area,
  adding them to the sprites already there.  If the new sprites have the same
  name as existing one, the old ones are deleted.  The area should have enough
  space to hold both the original sprites and the loaded ones.
*/


extern os_error *Sprite_Save(sprite_area area, const char *filename);
/*
  This is a veneer to OS_SpriteOp 12.

  It saves all the sprites in the given sprite area with the given filename.
*/


extern os_error *Sprite_Create(sprite_area area, const char *name,
                               int haspalette,
                               int width, int height,
                               screen_modeval screenmode);
/*
  This is a veneer to OS_SpriteOp 15.

  This creates a new sprite in the given sprite area, with the properties
  specified.
*/


extern os_error *Sprite_Select(sprite_area area, const char *name,
                               sprite_header **spriteptr);
/*
  This is a veneer to OS_SpriteOp 24.

  It selects the sprite with the given name from the given sprite area for
  subsequent plotting.  It fills the (sprite_header *) pointed to by
  spriteptr with a pointer to the sprite if it is found.  This is only valid
  until the next time the sprite area is rearranged (such as by deleting a
  sprite).
*/


extern os_error *Sprite_SelectP(sprite_area area,
                                const sprite_header *sprite);
/*
  This is a veneer to OS_SpriteOp 24.

  It selects the sprite with the given pointer of that sprite from the given
  sprite area for subsequent plotting.  This is only valid until the next
  time the sprite area is rearranged (such as by deleting a sprite).
*/


extern os_error *Sprite_Delete(sprite_area area, const char *name);
extern os_error *Sprite_DeleteP(sprite_area area,
                                const sprite_header *sprite);
/*
  This is a veneer to OS_SpriteOp 25.

  It deletes the given sprite from the given sprite area.
*/


extern os_error *Sprite_Rename(sprite_area area,
                               const char *oldname,
                               const char *newname);
/*
  This is a veneer to OS_SpriteOp 26.

  It changes the name of the sprite called 'oldname' in the given sprite area
  to 'newname'.  It produces an error if there is already a sprite 'newname'
  in the area.  Bear in mind that sprite names can be no more than 12
  characters long.
*/


extern os_error *Sprite_RenameP(sprite_area area,
                                sprite_header *oldsprite,
                                const char *newname);
/*
  This is a veneer to OS_SpriteOp 26.

  It changes the name of the sprite at pointer 'oldsprite' in the given
  sprite area to 'newname'.  It produces an error if there is already a
  sprite 'newname' in the area.  Bear in mind that sprite names can be no
  more than 12 characters long.
*/


extern os_error *Sprite_Copy(sprite_area area,
                             const char *oldname,
                             const char *newname);
/*
  This is a veneer to OS_SpriteOp 27.

  It makes a copy of the sprite called 'oldname' in the the given sprite
  area.  The new sprite is called 'newname'.  It produces an error if there
  is already a sprite called 'newname' in the sprite area.
*/


extern os_error *Sprite_CopyP(sprite_area area,
                              const sprite_header *oldsprite,
                              const char *newname);
/*
  This is a veneer to OS_SpriteOp 27.

  It makes a copy of the sprite at pointer 'oldsprite' in the the given
  sprite area.  The new sprite is called 'newname'.  It produces an error
  if there is already a sprite called 'newname' in the sprite area.
*/


extern os_error *Sprite_Get(sprite_area area,
                            const char *name, int with_palette,
                            int left, int bottom, int right, int top,
                            sprite_header **sprite_ptr);
/*
  This is a veneer to OS_SpriteOp 16.

  It creates a new sprite with name 'name' in the given sprite area,
  and copies the contents of the given screen rectangle into it.  If a
  sprite called 'name' already exists, it is replaced with this one.
  A pointer to the new sprite is returned in 'sprite_ptr'.
*/


extern os_error *Sprite_Plot(sprite_area area,
                             const char *name,  /* "PutUser" */
                             int x, int y, int plot_action);
/*
  This is a veneer for OS_SpriteOp 34.

  This plots the sprite 'name' from the given sprite area with its
  bottom-left corner at the given screen coordinates.

  The plot action is one of:
    0 overwrite the screen colour with the sprite colour,
    1 OR the screen colour with the sprite colour,
    2 AND the screen colour with the sprite colour,
    3 EOR the screen colour with the sprite colour,
    4 invert the screen colour,
    5 leave screen colour unchanged,
    6 AND the screen colour with the NOT of the sprite colour,
    7 OR the screen colour with the NOT of the sprite colour.

  You can set it to plot the sprite using the mask by bitwise ORing these
  values with 8.
*/


extern os_error *Sprite_PlotP(sprite_area area,
                              const sprite_header *sprite,
                              int x, int y, int plot_action);
/*
  This is a veneer for OS_SpriteOp 34.

  This plots the sprite at pointer 'sprite' from the given sprite area with
  its bottom-left corner at the given screen coordinates.

  The plot action is one of:
    0 overwrite the screen colour with the sprite colour,
    1 OR the screen colour with the sprite colour,
    2 AND the screen colour with the sprite colour,
    3 EOR the screen colour with the sprite colour,
    4 invert the screen colour,
    5 leave screen colour unchanged,
    6 AND the screen colour with the NOT of the sprite colour,
    7 OR the screen colour with the NOT of the sprite colour.

  You can set it to plot the sprite using the mask by bitwise ORing these
  values with 8.
*/


#define sprite_POINTERSHAPE1 (1)
#define sprite_POINTERSHAPE2 (2)
#define sprite_POINTERSHAPE3 (3)
#define sprite_POINTERSHAPE4 (4)
/*
  Used with Sprite_SetPointerShape and Sprite_SetPointerShapeP argument
  'bits' to specify which pointer shape needs to be set.
*/

#define sprite_POINTERSHAPE_IGNORESHAPEDATA (1<<4)
/*
  Used with Sprite_SetPointerShape and Sprite_SetPointerShapeP to defer
  the pointer shape setting.
*/

#define sprite_POINTERSHAPE_IGNOREPALETTE (1<<5)
/*
  Used with Sprite_SetPointerShape and Sprite_SetPointerShapeP to prevent
  the sprite palette being taken.
*/

#define sprite_POINTERSHAPE_IGNORESHAPE (1<<6)
/*
  Used with Sprite_SetPointerShape and Sprite_SetPointerShapeP to prevent
  the pointer shape to be reprogrammed.
*/

extern os_error *Sprite_SetPointerShape(sprite_area area,
                                        const char *name,
                                        unsigned int bits,
                                        const wimp_point *active_point,
                                        const sprite_scalefactors *scale,
                                        const void *pixtrans);
extern os_error *Sprite_SetPointerShapeP(sprite_area area,
                                         const sprite_header *sprite,
                                         unsigned int bits,
                                         const wimp_point *active_point,
                                         const sprite_scalefactors *scale,
                                         const void *pixtrans);
/*
  This is a veneer for OS_SpriteOp 36.
  
  This sets up the pointer shape using given sprite.  The pointer shape
  will be scaled using the scaling factors pointed to by 'scale' and the
  pixel translation table given by 'pixtrans'.
*/


extern os_error *Sprite_ReadInfo(sprite_area area, const char *name,
                                 sprite_info *info);
extern os_error *Sprite_ReadInfoP(sprite_area area,
                                  const sprite_header *sprite,
                                  sprite_info *info);
/*
  This is a veneer for OS_SpriteOp 40.

  This returns information (in 'info') about the sprite specified, giving its
  width and height in pixels, whether the sprite has a mask and the screen
  mode in which it was defined.
*/


extern os_error *Sprite_Redirect(sprite_area area, const char *name,
                                 void *savearea,
                                 sprite_outputstate *oldstate);
extern os_error *Sprite_RedirectP(sprite_area area,
                                  const sprite_header *sprite,
                                  void *savearea,
                                  sprite_outputstate *oldstate);
/*
  This is a veneer for OS_SpriteOp 60.

  It redirects VDU output into the given sprite.  You should pass a pointer
  to a sprite_outputstate block, which will be updated to hold the details
  of the old output state so that you can switch back using
  Sprite_UnRedirect.

  You should pass a save area block of the size given by a call to
  Sprite_ReadSaveAreaSize if you want to be able to switch output again and
  still return to this output state.  You can pass NULL if you don't wish to
  use a save area (ie. you are changing output to a sprite, performing a
  one-off action and then switching output back again).
*/


extern os_error *Sprite_UnRedirect(const sprite_outputstate *oldstate);
/*
  This is a veneer for OS_SpriteOp 60/61.

  You pass the output state you stored when you called Sprite_Redirect, and
  it sets the output back to that saved state.
*/


extern os_error *Sprite_RedirectMask(sprite_area area, const char *name,
                                     void *savearea,
                                     sprite_outputstate *oldstate);
extern os_error *Sprite_RedirectMaskP(sprite_area area,
                                      const sprite_header *sprite,
                                      void *savearea,
                                      sprite_outputstate *oldstate);
/*
  This is a veneer for OS_SpriteOp 61.

  This acts identically to Sprite_Redirect, except the output is to the
  mask.
*/


extern os_error *Sprite_ReadSaveAreaSize(sprite_area area, const char *name,
                                         int *saveareasize);
/*
  This is a veneer for OS_SpriteOp 62.

  Given a sprite area and name, it returns the size of save area you need to
  allocate for that sprite.  Use this with Sprite_Redirect.
*/


extern int Sprite_ReadControl(sprite_area area, int *size,
                              sprite_header **firstsprite,
                              sprite_header **firstfree);
/*
  This is a veneer for OS_SpriteOp 8.

  It reads information about the given sprite area.  It returns the total
  size of the area in bytes, a sprite pointer to the first sprite in the area
  and a pointer to the first free sprite in the area.

  If you aren't interested in any of the returned values, pass NULL pointers
  in.
*/


extern int Sprite_GetName(sprite_area area, const char *name, int spritenumber);
/*
  This is a veneer for OS_SpriteOp 13.

  Given a sprite area and the number of the sprite within that area (starting
  from one) it returns the sprite name in 'name'.  'name' should be at least
  13 chars long to be sure of fitting in the sprite name and terminator.

  It returns the length of the name.
*/


extern os_error *Sprite_CreateMask(sprite_area area, const char *name);
extern os_error *Sprite_CreateMaskP(sprite_area area, sprite_header *sprite);
/*
  This is a veneer for OS_SpriteOp 29.

  This creates a mask for the given sprite.
*/


extern os_error *Sprite_RemoveMask(sprite_area area, const char *name);
extern os_error *Sprite_RemoveMaskP(sprite_area area, sprite_header *sprite);
/*
  This is a veneer for OS_SpriteOp 30.

  This removes a mask from the given sprite.
*/


extern os_error *Sprite_InsertRow(sprite_area area, const char *name,
                                  int pos);
extern os_error *Sprite_InsertRowP(sprite_area area, sprite_header *sprite,
                                   int pos);
/*
  This is a veneer for OS_SpriteOp 31.

  This inserts a row in the given sprite at the given position, and shuffles
  the rest upwards.  Rows are numbered from the bottom upwards with the
  bottom row being zero.  Any value larger than the height of the sprite will
  generate an error.
*/


extern os_error *Sprite_DeleteRow(sprite_area area, const char *name,
                                  int pos);
extern os_error *Sprite_DeleteRowP(sprite_area area, sprite_header *sprite,
                                   int pos);
/*
  This is a veneer for OS_SpriteOp 32.

  This deletes a row in the given sprite at the given position, and shuffles
  the rest to fill the gap.  Rows are numbered from the bottom upwards with
  the bottom row being zero.  Any value larger than or equal to the height of
  the sprite will generate an error.
*/


extern os_error *Sprite_FlipX(sprite_area area, const char *name);
extern os_error *Sprite_FlipXP(sprite_area area,
                               const sprite_header *sprite);
/*
  This is a veneer for OS_SpriteOp 33.

  This flips the sprite about the x axis (ie. top to bottom).
*/


#define sprite_APPENDVERT 1
/*
  Used with Sprite_Append and Sprite_AppendP to denote that sprites should be
  appended vertically.
*/

#define sprite_APPENDHORIZ 0
/*
  Used with Sprite_Append and Sprite_AppendP to denote that sprites should be
  appended horizontally.
*/

extern os_error *Sprite_Append(sprite_area area, const char *name1,
                               const char *name2,
                               int dir);
/*
  This is a veneer for OS_SpriteOp 35.

  This function joins two sprites together. 'name2' is deleted and joined
  onto 'name1'. If 'dir' is sprite_APPENDHORIZ, they are joined horizontally
  and must have the same height.  If 'dir' is sprite_APPENDVERT they are
  joined vertically and must have the same width.
*/

extern os_error *Sprite_AppendP(sprite_area area, sprite_header *sprite1,
                                const char *name2,
                                int dir);
/*
  This is a veneer for OS_SpriteOp 35.

  This function joins two sprites together. 'name2' is deleted and joined
  onto 'sprite1'. If 'dir' is sprite_APPENDHORIZ, they are joined
  horizontally and must have the same height.  If 'dir' is sprite_APPENDVERT
  they are joined vertically and must have the same width.
*/


extern int Sprite_CreatePalette(sprite_area area, const char *name,
                                BOOL big);
extern int Sprite_CreatePaletteP(sprite_area area, sprite_header *sprite,
                                 BOOL big);
/*
  This is a veneer for OS_SpriteOp 37.

  This creates a palette for the given sprite, returning the size of the
  created palette (0 in case this call failed).  If 'big' is TRUE then a 256
  entry palette is created.
*/


extern int Sprite_ReadPaletteSize(sprite_area area, const char *name);
extern int Sprite_ReadPaletteSizeP(sprite_area area,
                                   const sprite_header *sprite);
/*
  This is a veneer for OS_SpriteOp 37.

  This function reads the palette size for given sprite.  It returns 0
  when the sprite doesn't have a palette or when an error happened.
*/


extern os_error *Sprite_RemovePalette(sprite_area area, const char *name);
extern os_error *Sprite_RemovePaletteP(sprite_area area,
                                       const sprite_header *sprite);
/*
  This is a veneer for OS_SpriteOp 37.

  This removes the palette from the given sprite.
*/


extern os_error *Sprite_ReadPixel(sprite_area area, const char *name,
                                  const wimp_point *pos,
                                  int *colour, int *tint);
extern os_error *Sprite_ReadPixelP(sprite_area area,
                                   const sprite_header *sprite,
                                   const wimp_point *pos,
                                   int *colour, int *tint);
/*
  This is a veneer for OS_SpriteOp 41.

  This returns the colour of the pixel at the given point in the given
  sprite.  The origin is at the bottom left of the sprite.
*/


extern os_error *Sprite_WritePixel(sprite_area area, const char *name,
                                   const wimp_point *pos,
                                   int colour, int tint);
extern os_error *Sprite_WritePixelP(sprite_area area,
                                    const sprite_header *sprite,
                                    const wimp_point *pos,
                                    int colour, int tint);
/*
  This is a veneer for OS_SpriteOp 42.

  This function sets the colour and tint of the pixel at 'pos' in the given
  sprite.  The origin is at the bottom left of the sprite.
*/


extern BOOL Sprite_ReadMask(sprite_area area, const char *name,
                            const wimp_point *pos);
extern BOOL Sprite_ReadMaskP(sprite_area area, const sprite_header *sprite,
                             const wimp_point *pos);
/*
  This is a veneer for OS_SpriteOp 43.

  It returns the value of the mask at the given point in the given sprite,
  TRUE if solid, FALSE if transparent.
*/


extern os_error *Sprite_WriteMask(sprite_area area, const char *name,
                                  const wimp_point *pos, BOOL solid);
extern os_error *Sprite_WriteMaskP(sprite_area area,
                                   const sprite_header *sprite,
                                   const wimp_point *pos, BOOL solid);
/*
  This is a veneer for OS_SpriteOp 44.

  It sets the value of the mask at the given point in the given sprite,
  according to the value of 'solid'.
*/


extern os_error *Sprite_InsertColumn(sprite_area area, const char *name,
                                     int pos);
extern os_error *Sprite_InsertColumnP(sprite_area area,
                                      sprite_header *sprite,
                                      int pos);
/*
  This is a veneer for OS_SpriteOp 45.

  It inserts a column in the given sprite at the specified position, shifting
  all columns after it (if any) to the right.  The left-hand column is
  numbered zero.
*/


extern os_error *Sprite_DeleteColumn(sprite_area area, const char *name,
                                     int pos);
extern os_error *Sprite_DeleteColumnP(sprite_area area,
                                      sprite_header *sprite,
                                      int pos);
/*
  This is a veneer for OS_SpriteOp 46.

  It deletes the specified column in the given sprite, shifting all rows
  after it (if there are any) to the left.  The left-hand column is numbered
  zero.
*/


extern os_error *Sprite_FlipY(sprite_area area, const char *name);
extern os_error *Sprite_FlipYP(sprite_area area,
                               const sprite_header *sprite);
/*
  This is a veneer to OS_SpriteOp 47.

  It flips the given sprite about the y axis (ie. left to right).
*/


extern os_error *Sprite_PlotMask(sprite_area area, const char *name,
                                 const wimp_point *pos);
extern os_error *Sprite_PlotMaskP(sprite_area area,
                                  const sprite_header *sprite,
                                  const wimp_point *pos);
/*
  This is a veneer to OS_SpriteOp 49.

  It plots the given sprite's *mask* at the given screen location using the
  current background graphics colour and action, only plotting those pixels
  for which the mask is 1 (solid).  If there is no mask, a rectangle the
  size of the sprite is plotted instead.
*/


extern os_error *Sprite_PlotMaskScaled(sprite_area area, const char *name,
                                       const wimp_point *pos,
                                       const sprite_scalefactors *scale);
extern os_error *Sprite_PlotMaskScaledP(sprite_area area,
                                        const sprite_header *sprite,
                                        const wimp_point *pos,
                                        const sprite_scalefactors *scale);
/*
  This is a veneer to OS_SpriteOp 50.

  It plots the given sprite's mask at the given screen location using the
  current background graphics colour and action, only plotting those pixels
  for which the mask is 1 (solid).  The mask is scaled according to the
  scale factors pointed to by 'scale'.
*/


extern os_error *Sprite_PlotScaled(sprite_area area, const char *name,
                                   const wimp_point *pos,
                                   unsigned int plot_action,
                                   const sprite_scalefactors *scale,
                                   const void *pixtrans);
extern os_error *Sprite_PlotScaledP(sprite_area area,
                                    const sprite_header *sprite,
                                    const wimp_point *pos,
                                    unsigned int plot_action,
                                    const sprite_scalefactors *scale,
                                    const void *pixtrans);
/*
  This is a veneer to OS_SpriteOp 52.

  It plots the given sprite with its bottom left corner at the position 'pos'
  The sprite will be scaled using the scaling factors pointed to by 'scale'
  and the pixel translation table given by 'pixtrans'.

  'scale' and 'pixtrans' may be set to NULL to not use scaling or
  translation, repectively.
*/


extern os_error *Sprite_PlotGrey(sprite_area area, const char *name,
                                 const wimp_point *pos,
                                 const sprite_scalefactors *scale,
                                 const void *pixtrans);
extern os_error *Sprite_PlotGreyP(sprite_area area,
                                  const sprite_header *sprite,
                                  const wimp_point *pos,
                                  const sprite_scalefactors *scale,
                                  const void *pixtrans);
/*
  This is a veneer for OS_SpriteOp 53.

  This function is similar to Sprite_PlotScaled, except that it anti-aliases
  the sprite as it scales it. The sprite must have been defined in a 16
  colour mode, and the palette must be a linear grey-scale.

  This call is slow and should only be used when the quality is more
  important than speed. If this quality is needed for something that will be
  redrawn more than once, it is advisable to redraw it once into another
  sprite, using Sprite_Redirect, and then redraw to screen using this sprite.
*/


extern os_error *Sprite_WimpPlot(sprite_area area, const char *name,
                                 const wimp_point *pos,
                                 const convert_block *convert,
                                 int plot_action);
/*
  This function plots the given sprite with its bottom left corner at the
  *work area* coordinates 'pos' for the window whose convert block is given.
  All relevant scaling and colour translating is taken to plot the sprite
  as the Wimp would plot it in an icon.
*/


/* Sprite manipulation functions ------------------------------------------ */

typedef enum
{
  sprite_HASNOMASKPAL = 0x00,             /* Sprite has no mask or palette */
  sprite_HASNOPALMASK = 0x00,
  sprite_HASMASK      = 0x01,             /* Sprite has a mask only        */
  sprite_HASPAL       = 0x02,             /* Sprite has a palette only     */
  sprite_HASMASKPAL   = 0x03,             /* Has both mask and palette     */
  sprite_HASPALMASK   = 0x03,
  sprite_DEEPFORMAT   = 0x04              /* Deep format sprite            */
} spritemem_flags;
/*
  Various flags you use when working out the amount of memory needed to
  hold a given sprite with Sprite_MemorySize.  The names are self-explanatory,
  but just in case, PAL stands for palette and MASK for mask.
*/


extern unsigned int Sprite_MemorySize(unsigned int width,
                                      unsigned int height,
                                      screen_modeval mode,
                                      spritemem_flags flags);
/*
  This calculates and returns the amount of memory needed to hold a sprite
  defined by the given parameters.  The width and height are in pixels, 'mode'
  is the mode of the sprite (-1 for the current screen mode), and the flags
  determine whether the sprite has a palette and/or mask.

  Make sure you specifiy sprite_DEEPFORMAT for new code, otherwise masks
  will use far more memory than is required. 
*/
/*
  Example:

  Create a sprite area large enough for 3 identical sprites which are mode 12,
  34x17, and have their own palettes.

  sprite_areainfo *pUserSprites;

  size = 3 * Sprite_MemorySize( 34, 17, 12, sprite_HASPAL );
  size += sizeof( sprite_areainfo );
  pUserSprites = malloc( size );
*/


extern int Sprite_IconMemorySize(window_block *pWinBlock, icon_handle icon,
                                 screen_modeval mode,
                                 spritemem_flags flags,
                                 wimp_point *dimensions);
/*
  This calculates and returns the amount of memory needed to hold a sprite
  which will exactly fill the given Wimp icon.  If 'dimensions' is not NULL,
  then it will be filled with the width and height of the sprite in pixels
  too.  You can pass -1 for 'mode' to use the current screen mode.

  The window is passed in as a window block, and so this is really designed
  for use with templates rather than open windows.  If you want to use it
  with an open window, you will have to get a copy of the window block with
  Wimp_GetWindowInfo (and *not* Window_GetInfo) or else use the template
  the window was created from (with Template_Find or similar).
*/


extern unsigned int Sprite_MemorySizeBpp(unsigned int width,
                                         unsigned int height,
                                         unsigned int bpp,
                                         spritemem_flags flags);
/*
  As per above, but when bpp is specified instead of a mode
*/


extern sprite_area Sprite_LoadFile(const char *filename);
/*
  This function allocates memory for a sprite area with malloc, initialises
  it, and loads the specified sprite file into it.
*/


extern sprite_area Sprite_LoadFiles(unsigned int nooffiles,...);
/*
  This function allocates memory for a sprite area with malloc, initialises
  it, and loads 'nooffiles' sprite files into it.  The files to be loaded are
  given as pointers to pathnames in the variable argument list.
*/


#ifdef __cplusplus
}
#endif


#endif
