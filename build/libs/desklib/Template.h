/*
    ####             #    #     # #
    #   #            #    #       #          The FreeWare C library for
    #   #  ##   ###  #  # #     # ###             RISC OS machines
    #   # #  # #     # #  #     # #  #   ___________________________________
    #   # ####  ###  ##   #     # #  #
    #   # #        # # #  #     # #  #    Please refer to the accompanying
    ####   ### ####  #  # ##### # ###    documentation for conditions of use
    ________________________________________________________________________

    File:    Template.h
    Author:  Copyright © 1992, 1993, 1994 John Winters and Jason Williams
    Version: 1.06 (17 Apr 2005)
    Purpose: Loading, caching, and retrieval of window templates

*/


#ifndef __dl_template_h
#define __dl_template_h

#ifndef __dl_sprite_h
#include "DeskLib:Sprite.h"
#endif

#ifndef __dl_linklist_h
#include "DeskLib:LinkList.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif

/* Abstract */
/*
  This header provides a set of functions for loading, caching and
  retrieving window templates from Template files.
*/


extern void Template_Initialise(void);
/*
  This initialises the Template manager ready for use.  Be sure to call this
  before you call any other Template functions.
*/


extern window_block *Template_Find(const char *name);
/*
  This tries to find the named template.  If it finds it, it returns a
  pointer to the original template definition so you can alter it.  To
  actually use the template, call Template_Clone.

  If it cannot find the template, it returns NULL.
*/


extern window_block *Template_Clone(const char *name, int maxtitlesize);
/*
  This makes a copy of the named template and returns a pointer to it.  If
  it cannot find the template, it returns NULL.

  'maxtitlesize' is the number of bytes to reserve for the window's title.
  This should be set large enough to hold any title you intend to give to
  the window, because the only way to extend it is to delete the window and
  recreate it.  Possible 'special' values for 'maxtitlesize' are
  template_TITLEDEFAULT which will allocate 260 bytes (enough for a pathname
  and an asterisk), and template_TITLEMIN which will allocate the space set
  in the template file.

  When you are finished with the clone, use Template_Free to release the
  memory it is using.
*/


#define template_TITLEDEFAULT (0)
/*
  This will cause the window creation function to allocate 260 bytes for
  the window title, so it will be able to hold a pathname (generally
  assumed to be 256 bytes or smaller) with a little space for an asterisk
  to mark altered contents or similar.
*/


#define template_TITLEMIN     (-1)
/*
  This will cause the window creation function to allocate the same amount
  of memory for the window title as was set in the template file.  This is
  the recommended default for windows whose titles will not be changed.
*/


extern void Template_Free(window_block **windowdef);
/*
  This frees the memory used by a template cloned with Template_Clone.
  It must *only* be used with such templates, never on an original
  template or it *will* crash your application or worse.

  Pass in a pointer to a pointer to a window block - this allows the function
  to set the pointer to NULL so you won't try an use it after freeing it.
  It freed all the memory used by the window including indirected data.
  This means that all such data should have been allocated with malloc or
  else the icons deleted before calling this function.
*/


extern void Template_Delete(const char *name);
/*
  Deletes the named template from the list of available templates, freeing
  any memory claimed by the template. This will "pull the rug out from
  under" any windows created using this template, and your program will
  crash if any such windows are open.

  However, any windows created using Template_Clone'd templates will
  be unaffected. (Moral of the story: Always use Template_Clone)

  Pass in the identifier string of the template you wish to delete.
*/


extern void Template_ClearAll(void);
/*
  Clears ALL currently held templates. (equivalent to you calling
  Template_Delete() for each template in turn)

  Also releases all fonts that were in use by the loaded templates.
*/


extern void Template_LoadFile(const char *leafname);
/*
  This loads all template definitions from the given file (which should be
  on the resource_pathname for your task) into the Template cache.  If any
  errors occur, they go to Error_ReportFatal or similar, as they tend to
  be unfixable.

  After loading the file, use Template_Clone to get a copy of any of the
  templates for use by giving the name as specified in the file.  If you don't
  want to keep old templates around after you've finished with them call
  Template_Delete or Template_ClearAll.  If you want to use outline fonts
  in your windows make sure you call Template_UseOutlineFonts *before* loading
  the file.
*/


extern void Template_UseOutlineFonts(void);
/*
  Call this before calling Template_LoadFile if you wish to use outline fonts
  in your windows.  This is used less often now that RISC OS has a desktop
  font which is used automatically anyway.  It allocates a font usage array
  and adds an atexit handler to free fonts you are using when your program
  quits.
*/


extern void Template_UseSpriteArea(sprite_area area);
/*
  This function sets the sprite area which will be used for all future
  calls to Template_Find or Template_Clone.  Set this to 1 to use the
  Wimp sprite pool (this is the default setting).
*/


extern void Template_LinkSpriteArea(const char *identifier, sprite_area area);
/*
  This sets the given single window template to use the given sprite area.
  Set this to 1 to use the Wimp sprite pool (this is the default setting).
*/




/*
  The rest of this file is not for user consumption - the declarations are
  used only by the Template_ functions and Window_ModeChange().
  Window_ModeChange() needs to get inside the template data to modify
  fonts handles.
*/

/* haddoc ignore on */

typedef struct
{
  linklist_header header;
  char            identifier[wimp_MAXNAME + 1];
  window_block    *windowdef;
  int             dataoffset;
  int             templatesize;    /* size of window+icons+indirect data */
  int             indirectsize;    /* size of expanded indirect data     */
  char            *indirectdata;
} template_record;

extern linklist_header template_list;
extern font_array      *template_fontarray;

/* haddoc ignore off */

#ifdef __cplusplus
}
#endif


#endif
