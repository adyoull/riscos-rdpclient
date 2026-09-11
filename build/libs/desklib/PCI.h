/*
    ####             #    #     # #
    #   #            #    #       #          The FreeWare C library for
    #   #  ##   ###  #  # #     # ###             RISC OS machines
    #   # #  # #     # #  #     # #  #   ___________________________________
    #   # ####  ###  ##   #     # #  #
    #   # #        # # #  #     # #  #    Please refer to the accompanying
    ####   ### ####  #  # ##### # ###    documentation for conditions of use
    ________________________________________________________________________

    File:    PCI.h
    Author:  Copyright © 2003 Peter Naulls
    Version: 1.01 (17 Apr 2005)
    Purpose: Veneers for RISC OS 5 PCI SWIs
*/

#ifndef __dl_pci_h
#define __dl_pci_h

#ifndef __dl_core_h
#include "DeskLib:Core.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif


/* Abstract */
/*
   Warning: This API has not yet been implemented.  Do not use.

   These declarations are essentially just wrappers for the PCI SWIs.
   In time, we might add more complex functionality.

   The PCI Manager allocates IDs for each PCI device in the computer.
   You can find the maximum ID with PCI_ReturnNumber, and use values up to
   and including that for functions taking handles.
*/


typedef unsigned int pci_handle;

typedef struct {
  int access     :  4;
  int bufferable :  1;
  int cacheable  :  1;
  int policy     :  3;
  int access_sp  :  1;
  int unused     : 20;
  int memory     :  1;
  int query      :  1;
} pci_aflags;

typedef struct {
  pci_aflags    flags;     /* Input values */
  unsigned int  index;
  unsigned int  flags;    /* Output values */
  unsigned int  address;
  unsigned int  size;
  void         *laddress;
} pci_address;

/*
This empty structure commented out to stop helpfile generation (haddoc) aborting

typedef struct {


} pci_info;
*/

/*
   Return PCI IDs for a given device.

   If ID is non-NULL, then bits 0-15 are set to vendor ID and
   bits 16-31 to device ID.

   If subID is non-NULL, then bits 0-15 are set to subsystem vendor ID and
   bits 16-31 to subsystem ID.
*/
extern void PCI_ReadID(pci_handle handle, unsigned int *ID,
                                          unsigned int *subID);


/*
   Read buflen bytes from the PCI header into the given buffer.
   buflen must be 4-256 and a multiple of 4.
*/
extern void PCI_ReadHeader(pci_handle handle, void *buffer,
                                              unsigned int buflen);


/*
   Return number of allocated function handles.  You can search from
   1 to the result of this inclusive for PCI devices.
*/
extern pci_handle PCI_ReturnNumber(void);


/*
   Similar to PCI_ReadID, but allows enumeration.  Use 0 for the first
   value, then the value returned for succesive calls.
*/
extern pci_handle PCI_EnumerateFuntions(pci_handle handle,
                                        unsigned int *ID,
                                        unsigned int *subID,
                                        unsigned int *classcode);

/*
   Read and write PCI I/O space.  The address passed can be found with
   PCI_HardwareAddress.  The size parameter is either 1, 2 or 4, indicating
   8, 16 or 32-bit access.
*/
extern unsigned int PCI_IORead(unsigned int address, unsigned int size);
extern void PCI_IOWrite(unsigned int address, unsigned int vaulue,
                                              unsigned int size);

/*
   Read and write PCI Memory space.  A buffer and length is passed
   for the access.  The only available flag is for reads - bit 3
   when set indicates it is prefetchable.
*/
extern void PCI_MemoryRead(unsigned int address, unsigned int length,
                           void *buffer, unsigned int flags);
extern void PCI_MemoryWrite(unsigned int address, unsigned int length,
                            void *buffer, unsigned int flags);


/*
   Read and write PCI configuration space.  The size parameter is either
   1, 2 or 4, indicating 8, 16 or 32-bit access.
*/
extern unsigned int PCI_ConfigurationRead(unsigned int address,
                                          unsigned int size,
                                          pci_handle handle);
extern void PCI_ConfigurationRead(unsigned int address,
                                  unsigned int value,
                                  unsigned int size,
                                  pci_handle handle);


extern unsigned int PCI_HardwareAddress(pci_address *address,
                                        pci_handle handle);


extern unsigned int PCI_SpecialCycle(unsigned int bus,
                                     unsigned int message);



extern pci_handle PCI_FindByID(int vendor, int device,
                               int subvendor, int subid);


extern pci_handle PCI_FindBuClass(pci_handle handle, unsigned int class_code,
                                  unsigned int mask);

extern void *PCI_RAMAlloca(size_t size, unsigned int alignment,
                                        unsigned int boundary,
                                        unsigned int *pci_address);

extern void PCI_RAMFree(void *address);


exetern void *PCI_LogicalAddress(pci_aflags flags, unsigned int pci_address,
                                 unsigned int length);


#ifdef __cplusplus
}
#endif

#endif
