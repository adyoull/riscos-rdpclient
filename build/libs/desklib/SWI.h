/*
    ####             #    #     # #
    #   #            #    #       #          The FreeWare C library for
    #   #  ##   ###  #  # #     # ###             RISC OS machines
    #   # #  # #     # #  #     # #  #   ___________________________________
    #   # ####  ###  ##   #     # #  #
    #   # #        # # #  #     # #  #    Please refer to the accompanying
    ####   ### ####  #  # ##### # ###    documentation for conditions of use
    ________________________________________________________________________

    File:    SWI.h
    Author:  Copyright © 1992 Jason Williams
    Version: 1.03 (17 Apr 2005)
    Purpose: Generic SWI call veneer function
*/


#ifndef __dl_swi_h
#define __dl_swi_h

#ifndef __dl_core_h
#include "DeskLib:Core.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif

/* Abstract */
/*
  This header provides one function - a general-purpose SWI calling veneer -
  and many macros giving names to SWI numbers.
*/


os_error *SWI(int numregsin, int numregsout, int swicode, ... );
/*
  This function provides a general-purpose SWI calling veneer.  It is not
  meant to be (and is not) the most efficient way of calling a SWI, but rather
  the most general one.  If you require faster SWI handling than this provides,
  you will need to construct an assembler veneer for the SWI (the Hourglass and
  WimpSWIs libraries are examples of these).

  You specify the number of arguments you are passing in, the number you want
  passed back and the SWI number to call (many of which are #defined by this
  header), and the parameters themselves.  It returns any error, or NULL if
  the SWI is executed successfully.

  The 'in' parameter list should be a list of integers, exactly the number
  specified as "numregsin". These will be passed to the SWI as registers
  0, 1, 2, ... A maximum of 10 such registers can be specified.

  The 'out' parameter list should be a list of pointers to result variables
  they will be assigned the register results from the SWI, in order.
  Pass in a NULL pointer for any of these if the result is to be ignored.

  Examples:

  SWI(0, 0, SWI_Hourglass_On);

  SWI(1, 0, SWI_Hourglass_Percentage, 33);

  To call the 'X' version of the SWI:
  SWI(0, 0, SWI_Hourglass_Smash | XOS_Bit);
*/

/*
  { int ramsprites, romsprites;
    \* Read rom sprite and ram sprite pool base addresses: *\
    SWI(0, 2, SWI_Wimp_BaseOfSprites, &romsprites, &ramsprites)

    \* Read ram sprite base ONLY *\
    SWI(0, 2, SWI_Wimp_BaseOfSprites, NULL, &ramsprites)
  }
*/


/* A few useful SWI numbers... */

#define SWI_XOS_Bit                        0x020000
/* Bitwise-OR this with a #defined SWI number to call the 'X' version */

#define XOS_Bit SWI_XOS_Bit
/* Bitwise-OR this with a #defined SWI number to call the 'X' version */

#define SWI_OS_WriteI                      0x000100

#define SWI_OS_WriteC                      0x000000
#define SWI_OS_WriteS                      0x000001
#define SWI_OS_Write0                      0x000002
#define SWI_OS_NewLine                     0x000003
#define SWI_OS_ReadC                       0x000004
#define SWI_OS_CLI                         0x000005
#define SWI_OS_Byte                        0x000006
#define SWI_OS_Word                        0x000007
#define SWI_OS_File                        0x000008
#define SWI_OS_Args                        0x000009
#define SWI_OS_BGet                        0x00000a
#define SWI_OS_BPut                        0x00000b
#define SWI_OS_GBPB                        0x00000c
#define SWI_OS_Find                        0x00000d
#define SWI_OS_ReadLine                    0x00000e
#define SWI_OS_Control                     0x00000f
#define SWI_OS_GetEnv                      0x000010
#define SWI_OS_Exit                        0x000011
#define SWI_OS_SetEnv                      0x000012
#define SWI_OS_IntOn                       0x000013
#define SWI_OS_IntOff                      0x000014
#define SWI_OS_CallBack                    0x000015
#define SWI_OS_EnterOS                     0x000016
#define SWI_OS_BreakPt                     0x000017
#define SWI_OS_BreakCtrl                   0x000018
#define SWI_OS_UnusedSWI                   0x000019
#define SWI_OS_UpdateMEMC                  0x00001a
#define SWI_OS_SetCallBack                 0x00001b
#define SWI_OS_Mouse                       0x00001c
#define SWI_OS_Heap                        0x00001d
#define SWI_OS_Module                      0x00001e
#define SWI_OS_Claim                       0x00001f
#define SWI_OS_Release                     0x000020
#define SWI_OS_ReadUnsigned                0x000021
#define SWI_OS_GenerateEvent               0x000022
#define SWI_OS_ReadVarVal                  0x000023
#define SWI_OS_SetVarVal                   0x000024
#define SWI_OS_GSInit                      0x000025
#define SWI_OS_GSRead                      0x000026
#define SWI_OS_GSTrans                     0x000027
#define SWI_OS_BinaryToDecimal             0x000028
#define SWI_OS_FSControl                   0x000029
#define SWI_OS_ChangeDynamicArea           0x00002a
#define SWI_OS_GenerateError               0x00002b
#define SWI_OS_ReadEscapeState             0x00002c
#define SWI_OS_EvaluateExpression          0x00002d
#define SWI_OS_SpriteOp                    0x00002e
#define SWI_OS_ReadPalette                 0x00002f
#define SWI_OS_ServiceCall                 0x000030
#define SWI_OS_ReadVduVariables            0x000031
#define SWI_OS_ReadPoint                   0x000032
#define SWI_OS_UpCall                      0x000033
#define SWI_OS_CallAVector                 0x000034
#define SWI_OS_ReadModeVariable            0x000035
#define SWI_OS_RemoveCursors               0x000036
#define SWI_OS_RestoreCursors              0x000037
#define SWI_OS_SWINumberToString           0x000038
#define SWI_OS_SWINumberFromString         0x000039
#define SWI_OS_ValidateAddress             0x00003a
#define SWI_OS_CallAfter                   0x00003b
#define SWI_OS_CallEvery                   0x00003c
#define SWI_OS_RemoveTickerEvent           0x00003d
#define SWI_OS_InstallKeyHandler           0x00003e
#define SWI_OS_CheckModeValid              0x00003f
#define SWI_OS_ChangeEnvironment           0x000040
#define SWI_OS_ClaimScreenMemory           0x000041
#define SWI_OS_ReadMonotonicTime           0x000042
#define SWI_OS_SubstituteArgs              0x000043
#define SWI_OS_PrettyPrint                 0x000044
#define SWI_OS_Plot                        0x000045
#define SWI_OS_WriteN                      0x000046
#define SWI_OS_AddToVector                 0x000047
#define SWI_OS_WriteEnv                    0x000048
#define SWI_OS_ReadArgs                    0x000049
#define SWI_OS_ReadRAMFsLimits             0x00004a
#define SWI_OS_ClaimDeviceVector           0x00004b
#define SWI_OS_ReleaseDeviceVector         0x00004c
#define SWI_OS_DelinkApplication           0x00004d
#define SWI_OS_RelinkApplication           0x00004e
#define SWI_OS_HeapSort                    0x00004f
#define SWI_OS_ExitAndDie                  0x000050
#define SWI_OS_ReadMemMapInfo              0x000051
#define SWI_OS_ReadMemMapEntries           0x000052
#define SWI_OS_SetMemMapEntries            0x000053
#define SWI_OS_AddCallBack                 0x000054
#define SWI_OS_ReadDefaultHandler          0x000055
#define SWI_OS_SetECFOrigin                0x000056
#define SWI_OS_SerialOp                    0x000057
#define SWI_OS_ReadSysInfo                 0x000058
#define SWI_OS_Confirm                     0x000059
#define SWI_OS_ChangedBox                  0x00005a
#define SWI_OS_CRC                         0x00005b
#define SWI_OS_ReadDynamicArea             0x00005c
#define SWI_OS_PrintChar                   0x00005d
#define SWI_OS_ChangeRedirection           0x00005e
#define SWI_OS_RemoveCallBack              0x00005f
#define SWI_OS_FindMemMapEntries           0x000060
#define SWI_OS_SetColour                   0x000061
#define SWI_OS_Pointer                     0x000064
#define SWI_OS_ScreenMode                  0x000065
#define SWI_OS_DynamicArea                 0x000066
#define SWI_OS_Memory                      0x000068
#define SWI_OS_ClaimProcessorVector        0x000069
#define SWI_OS_Reset                       0x00006a
#define SWI_OS_MMUControl                  0x00006b
#define SWI_OS_ConvertStandardDateAndTime  0x0000c0
#define SWI_OS_ConvertDateAndTime          0x0000c1
#define SWI_OS_ConvertHex1                 0x0000d0
#define SWI_OS_ConvertHex2                 0x0000d1
#define SWI_OS_ConvertHex4                 0x0000d2
#define SWI_OS_ConvertHex6                 0x0000d3
#define SWI_OS_ConvertHex8                 0x0000d4
#define SWI_OS_ConvertCardinal1            0x0000d5
#define SWI_OS_ConvertCardinal2            0x0000d6
#define SWI_OS_ConvertCardinal3            0x0000d7
#define SWI_OS_ConvertCardinal4            0x0000d8
#define SWI_OS_ConvertInteger1             0x0000d9
#define SWI_OS_ConvertInteger2             0x0000da
#define SWI_OS_ConvertInteger3             0x0000db
#define SWI_OS_ConvertInteger4             0x0000dc
#define SWI_OS_ConvertBinary1              0x0000dd
#define SWI_OS_ConvertBinary2              0x0000de
#define SWI_OS_ConvertBinary3              0x0000df
#define SWI_OS_ConvertBinary4              0x0000e0
#define SWI_OS_ConvertSpacedCardinal1      0x0000e1
#define SWI_OS_ConvertSpacedCardinal2      0x0000e2
#define SWI_OS_ConvertSpacedCardinal3      0x0000e3
#define SWI_OS_ConvertSpacedCardinal4      0x0000e4
#define SWI_OS_ConvertSpacedInteger1       0x0000e5
#define SWI_OS_ConvertSpacedInteger2       0x0000e6
#define SWI_OS_ConvertSpacedInteger3       0x0000e7
#define SWI_OS_ConvertSpacedInteger4       0x0000e8
#define SWI_OS_ConvertFixedNetStation      0x0000e9
#define SWI_OS_ConvertNetStation           0x0000ea
#define SWI_OS_ConvertFixedFileSize        0x0000eb
#define SWI_OS_ConvertFileSize             0x0000ec
#define SWI_IIC_Control                    0x000240
#define SWI_Econet_CreateReceive           0x040000
#define SWI_Econet_ExamineReceive          0x040001
#define SWI_Econet_ReadReceive             0x040002
#define SWI_Econet_AbandonReceive          0x040003
#define SWI_Econet_WaitForReception        0x040004
#define SWI_Econet_EnumerateReceive        0x040005
#define SWI_Econet_StartTransmit           0x040006
#define SWI_Econet_PollTransmit            0x040007
#define SWI_Econet_AbandonTransmit         0x040008
#define SWI_Econet_DoTransmit              0x040009
#define SWI_Econet_ReadLocalStationAndNet  0x04000a
#define SWI_Econet_ConvertStatusTOString   0x04000b
#define SWI_Econet_ConvertStatusToError    0x04000c
#define SWI_Econet_ReadProtection          0x04000d
#define SWI_Econet_SetProtection           0x04000e
#define SWI_Econet_ReadStationNumber       0x04000f
#define SWI_Econet_PrintBanner             0x040010
#define SWI_Econet_ReleasePort             0x040012
#define SWI_Econet_AllocatePort            0x040013
#define SWI_Econet_DeAllocatePort          0x040014
#define SWI_Econet_ClaimPort               0x040015
#define SWI_Econet_StartImmediate          0x040016
#define SWI_Econet_DoImmediate             0x040017
#define SWI_NetFS_ReadFSNumber             0x040040
#define SWI_NetFS_SetFSNumber              0x040041
#define SWI_NetFS_ReadFSName               0x040042
#define SWI_NetFS_SetFSName                0x040043
#define SWI_NetFS_ReadCurrentContext       0x040044
#define SWI_NetFS_SetCurrentContext        0x040045
#define SWI_NetFS_ReadFSTimeouts           0x040046
#define SWI_NetFS_SetFSTimeouts            0x040047
#define SWI_NetFS_DoFSOp                   0x040048
#define SWI_NetFS_EnumerateFSList          0x040049
#define SWI_NetFS_EnumerateFS              0x04004a
#define SWI_NetFS_ConvertDate              0x04004b
#define SWI_NetFS_DoFSOpToGivenFS          0x04004c
#define SWI_Wimp_TextOp                    0x0400F9
#define SWI_Wimp_SetWatchdogState          0x0400FA
#define SWI_Wimp_Extend                    0x0400FB
#define SWI_Wimp_ResizeIcon                0x0400FC
#define SWI_Sound_Configure                0x040140
#define SWI_Sound_Enable                   0x040141
#define SWI_Sound_Stereo                   0x040142
#define SWI_Sound_Speaker                  0x040143
#define SWI_Sound_Volume                   0x040180
#define SWI_Sound_SoundLog                 0x040181
#define SWI_Sound_LogScale                 0x040182
#define SWI_Sound_InstallVoice             0x040183
#define SWI_Sound_RemoveVoice              0x040184
#define SWI_Sound_AttachVoice              0x040185
#define SWI_Sound_ControlPacked            0x040186
#define SWI_Sound_Tuning                   0x040187
#define SWI_Sound_Pitch                    0x040188
#define SWI_Sound_Control                  0x040189
#define SWI_Sound_AttachNamedVoice         0x04018a
#define SWI_Sound_ReadControlBlock         0x04018b
#define SWI_Sound_WriteControlBlock        0x04018c
#define SWI_Sound_QInit                    0x0401c0
#define SWI_Sound_QSchedule                0x0401c1
#define SWI_Sound_QRemove                  0x0401c2
#define SWI_Sound_QFree                    0x0401c3
#define SWI_Sound_QSDispatch               0x0401c4
#define SWI_Sound_QTempo                   0x0401c5
#define SWI_Sound_QBeat                    0x0401c6
#define SWI_Sound_QInterface               0x0401c7
#define SWI_NetPrint_ReadPSNumber          0x040200
#define SWI_NetPrint_SetPSNumber           0x040201
#define SWI_NetPrint_ReadPSName            0x040202
#define SWI_NetPrint_SetPSName             0x040203
#define SWI_NetPrint_ReadPSTimeouts        0x040204
#define SWI_NetPrint_SetPSTimeouts         0x040205
#define SWI_ADFS_DiscOp                    0x040240
#define SWI_ADFS_HDC                       0x040241
#define SWI_ADFS_Drives                    0x040242
#define SWI_ADFS_FreeSpace                 0x040243
#define SWI_ADFS_Retries                   0x040244
#define SWI_ADFS_DescribeDisc              0x040245
#define SWI_Podule_ReadID                  0x040280
#define SWI_Podule_ReadHeader              0x040281
#define SWI_Podule_EnumerateChunks         0x040282
#define SWI_Podule_ReadChunk               0x040283
#define SWI_Podule_ReadBytes               0x040284
#define SWI_Podule_WriteBytes              0x040285
#define SWI_Podule_CallLoader              0x040286
#define SWI_Podule_RawRead                 0x040287
#define SWI_Podule_RawWrite                0x040288
#define SWI_Podule_HardwareAddress         0x040289
#define SWI_Podule_EnumerateChunksWithInfo 0x04028A
#define SWI_Podule_HardwareAddresses       0x04028B
#define SWI_Podule_ReturnNumber            0x04028C
#define SWI_Podule_ReadInfo                0x04028D
#define SWI_Podule_SetSpeed                0x04028E
#define SWI_WaveSynth_Load                 0x040300
#define SWI_Debugger_Disassemble           0x040380
#define SWI_FPEmulator_Version             0x040480
#define SWI_FileCore_DiscOp                0x040540
#define SWI_FileCore_Create                0x040541
#define SWI_FileCore_Drives                0x040542
#define SWI_FileCore_FreeSpace             0x040543
#define SWI_FileCore_FloppyStructure       0x040544
#define SWI_FileCore_DescribeDisc          0x040545
#define SWI_Shell_Create                   0x0405c0
#define SWI_Shell_Destroy                  0x0405c1
#define SWI_Draw_ProcessPath               0x040700
#define SWI_Draw_ProcessPathFP             0x040701
#define SWI_Draw_Fill                      0x040702
#define SWI_Draw_FillFP                    0x040703
#define SWI_Draw_Stroke                    0x040704
#define SWI_Draw_StrokeFP                  0x040705
#define SWI_Draw_StrokePath                0x040706
#define SWI_Draw_StrokePathFP              0x040707
#define SWI_Draw_FlattenPath               0x040708
#define SWI_Draw_FlattenPathFP             0x040709
#define SWI_Draw_TransformPath             0x04070a
#define SWI_Draw_TransformPathFP           0x04070b
#define SWI_ColourTrans_GenerateTable      0x040763
#define SWI_RamFS_DiscOp                   0x040780
#define SWI_RamFS_Drives                   0x040782
#define SWI_RamFS_FreeSpace                0x040783
#define SWI_RamFS_DescribeDisc             0x040785
#define SWI_TaskManager_TaskNameFromHandle 0x042680
#define SWI_TaskManager_EnumerateTasks     0x042681
#define SWI_TaskManager_Shutdown           0x042682

/* DeviceFS */
#define SWI_DeviceFS_Register              0x042740
#define SWI_DeviceFS_Deregister            0x042741
#define SWI_DeviceFS_RegisterObjects       0x042742
#define SWI_DeviceFS_DeregisterObjects     0x042743
#define SWI_DeviceFS_CallDevice            0x042744
#define SWI_DeviceFS_Threshold             0x042745
#define SWI_DeviceFS_ReceivedCharacter     0x042746
#define SWI_DeviceFS_TransmitCharacter     0x042747

/* Acorn URI system SWIs */
#define SWI_URI_Version                    0x04E380
#define SWI_URI_Dispatch                   0x04E381
#define SWI_URI_RequestURI                 0x04E382
#define SWI_URI_InvalidateURI              0x04E383

/* PCI Manager */
#define SWI_PCI_ReadID                     0x050380
#define SWI_PCI_ReadHeader                 0x050381
#define SWI_PCI_ReturnNumber               0x050382
#define SWI_PCI_EnumerateFunctions         0x050383
#define SWI_PCI_IORead                     0x050384
#define SWI_PCI_IOWrite                    0x050385
#define SWI_PCI_MemoryRead                 0x050386
#define SWI_PCI_MemoryWrite                0x050387
#define SWI_PCI_ConfigurationRead          0x050388
#define SWI_PCI_ConfigurationWrite         0x050389
#define SWI_PCI_HardwareAddress            0x05038A
#define SWI_PCI_ReadInfo                   0x05038B
#define SWI_PCI_SpecialCycle               0x05038C
#define SWI_PCI_FindByLocation             0x05038D
#define SWI_PCI_FindByID                   0x05038E
#define SWI_PCI_FindByClass                0x05038F
#define SWI_PCI_RAMAlloc                   0x050390
#define SWI_PCI_RAMFree                    0x050391
#define SWI_PCI_LogicalAddress             0x050392

/* Tinct */
#define SWI_Tinct_PlotAlpha                0x057240
#define SWI_Tinct_PlotScaledAlpha          0x057241
#define SWI_Tinct_Plot                     0x057242
#define SWI_Tinct_PlotScaled               0x057243
#define SWI_Tinct_ConvertSprite            0x057244
#define SWI_Tinct_AvailableFeatures        0x057245
#define SWI_Tinct_Compress                 0x057246
#define SWI_Tinct_Decompress               0x057247

#ifdef __cplusplus
}
#endif


#endif
