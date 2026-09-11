/*
    ####             #    #     # #
    #   #            #    #       #          The FreeWare C library for
    #   #  ##   ###  #  # #     # ###             RISC OS machines
    #   # #  # #     # #  #     # #  #   ___________________________________
    #   # ####  ###  ##   #     # #  #
    #   # #        # # #  #     # #  #    Please refer to the accompanying
    ####   ### ####  #  # ##### # ###    documentation for conditions of use
    ________________________________________________________________________

    File:    Resource.h
    Author:  Copyright © 1992, 1993, 1994 Jason Williams
    Version: 1.03 (17 Apr 2005)
    Purpose: Resource file (files within user application directory)
             management functions
    Mods:    Julian Smith 20 Mar 1995
             Added function veneers to global vars if _desklib_DLL is
             defined.
*/


#ifndef __dl_resource_h
#define __dl_resource_h

#ifndef __dl_sprite_h
#include "DeskLib:Sprite.h"
#endif


#ifdef __cplusplus
extern "C" {
#endif


/* Abstract */
/*
  This header declares functions for handling Resource files inside our
  application directory (spritefiles, for instance), and functions for
  dealing with Choices files (user options) which work out the correct
  location for such files when loading and saving options.
*/

#define choices_SINGLE      FALSE     /* Used with Resource_ChoicesInit() */
#define choices_MULTIPLE    TRUE      /* Used with Resource_ChoicesInit() */

#define choices_READ        FALSE     /* Used with Resource_ChoicesPath() */
#define choices_WRITE       TRUE      /* Used with Resource_ChoicesPath() */


extern char resource_pathname[32];
/*
  This string is used as a prefix for all pathnames in DeskLib that
  load resource files (eg. Template_LoadFile).  It is set to either
  "<Name$Dir>." or "Name:", depending on how you initialise the
  resource manager.
*/

extern sprite_area resource_sprites;
/*
  This variable points to a sprite area containing sprites belonging to
  your task.  If you have not loaded any with Resource_LoadSprites, it
  will point to the wimp sprite area.
*/


extern void Resource_Initialise(const char *respath);
/*
  This initialises the resource manager, setting resource_pathname to
  "<respath$dir>.", eg. if you have set "appname$dir" to point to your
  application directory you should pass in "appname".

  This tells DeskLib where to look for your resources by prepending
  resource_pathname to a given leafname to create a full resource
  pathname.  This adds very little code size to your program, and makes
  it a lot easier to change the resource directory at a later time.
*/


extern void Resource_InitialisePath(const char *respath);
/*
  This initialises the resource manager, setting resource_pathname to
  "respath:", eg. if you have set "appname:" to point to the directory
  (or directories) where your sprite files, templates, etc are stored,
  you should pass in "appname".

  This tells DeskLib where to look for your resources by prepending
  resource_pathname to a given leafname to create a full resource
  pathname.  This adds very little code size to your program, and makes
  it a lot easier to change the location of resources at a later time.

  Using a path has the advantage of being able to reference more than
  one directory, so you can have a language-specific directory and a
  default directory for resources, and this will enable DeskLib to load
  language-specific files first if they exist, and fall back to the
  default files if they don't.
*/


extern void Resource_LoadSprites(void);
/*
  This loads the program's "Sprites" file from the resource path, ready
  for use.  You should only call this after you have initialised the
  Resource system.  After loading the file, resource_sprites will point
  to the sprite area containing the sprites.

  To use these sprites for your templates, use
  Template_UseSpriteArea(resource_sprites); after calling this function
  and before creating your windows.

  If there is a problem loading the file, the error is reported with
  Error_ReportFatal.
*/


extern void Resource_ChoicesInit(const char *groupdir,
                                 const char *appname, BOOL multiple);
/*
  This initialises the Choices system, setting up the appropriate global
  variables.  This must be called before you use Resource_ChoicesPath.

  You set 'appname' to the name of the owning application (yours!).
  This will be used as the name of a single choices file if 'multiple'
  is choices_SINGLE, or as the name of a directory containing your
  choices files if 'multiple' is choices_MULTIPLE.  You can still use
  choices_MULTIPLE if you are using a single file, if you want that
  file placed in a subdirectory.

  If you want to use a parent directory (such as "WWW" or "MidiWays")
  for your choices, you can pass this in as 'groupdir'.  If this is
  NULL or a zero-length string no group dir will be used.

  This is based on the function given by Justin Fletcher in his Choices
  doc, though with additional functionality for grouping multiple choices
  files.
*/


extern void Resource_ChoicesPath(char *pathname,
                                 const char *leafname, BOOL write);
/*
  You pass in the leafname of the choices file you want to access, and
  whether you are reading the file or writing it.  It returns the correct
  filename in the buffer pointed to by 'pathname', based on the way you
  have set up the system with Resource_ChoicesInit.  You should make sure
  that the buffer is large enough to hold the resulting pathname.

  As per the Choices specification, when reading, a choices file is looked
  for first on the "Choices:" path and then inside the application directory,
  but when writing, the file should be written in "<Choices$Write>" if
  available, or inside the application if not.

  If the Choices system is in use and you have configured multiple files
  for your choices then the leafname given here is used as the filename
  inside your application's Choices directory.  If you have configured a
  single choices file, then the *application name* is used as the leafname
  of your file.  If you have specified a group directory (such as "WWW"),
  this is used as a "parent" directory to locate the file or files.

  If the Choices system cannot be used, then for a single file the leafname
  is the name of the file inside your application directory, and for multiple
  files it is the leafname of a file inside a "Choices" directory inside your
  application directory.

  If the Choices system cannot be used, the group directory is not used
  whether it is set or not.

  Examples (appname is "MyApp" and leafname is "!Config").

  With no groupdir and choices_SINGLE:
  "Choices:MyApp", or "<MyApp$Dir>.!Config" if that fails.

  With no groupdir and choices_MULTIPLE:
  "Choices:MyApp.!Config" or "<MyApp$Dir>.Choices.!Config"

  With groupdir="WWW" and choices_SINGLE:
  "Choices:WWW.MyApp" or "<MyApp$Dir>.!Config"

  With groupdir="WWW" and choices_MULTIPLE:
  "Choices:WWW.MyApp.!Config" or "<MyApp$Dir>.Choices.!Config"
*/


#ifdef __cplusplus
}
#endif


#endif
