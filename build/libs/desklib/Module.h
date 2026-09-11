/*
    ####             #    #     # #
    #   #            #    #       #          The FreeWare C library for
    #   #  ##   ###  #  # #     # ###             RISC OS machines
    #   # #  # #     # #  #     # #  #   ___________________________________
    #   # ####  ###  ##   #     # #  #
    #   # #        # # #  #     # #  #    Please refer to the accompanying
    ####   ### ####  #  # ##### # ###    documentation for conditions of use
    ________________________________________________________________________

    File:    Module.h
    Author:  Copyright © 1993 Jason Howat
    Version: 1.00 (23 Nov 1993)
    Purpose: Veneers for the OS_Module SWI.
*/

#ifndef __dl_module_h
#define __dl_module_h

#ifdef __cplusplus
extern "C" {
#endif


/* Abstract */
/*
  This header declares a pair of veneers for claiming memory from
  the RMA and releasing it again.
*/


extern os_error *Module_Claim(unsigned size, void **block);
/*
  This function claims an area of RMA space of the size specified and
  returns a pointer to it in *block.  If the function is successful it
  returns NULL.
*/


extern os_error *Module_Free(void *block);
/*
  This function releases a block of RMA space previously claimed using
  Module_Claim.  block is the pointer to this block.
*/


#ifdef __cplusplus
}
#endif

#endif
