#include "decl_defs.inc"
#include "config.h"

//######################################################
// DeclROM sRSRC directory - lowest address in ROM
//######################################################
.global DeclROMDir  // "entry" point for linker
.section .text

_sRsrcDir:
    OSLstEntry    sRsrc_Board, _sRsrc_Board       // board sRsrc
    OSLstEntry    sRsrc_CardDisk, _sRsrc_CardDisk // device sRsrc
    DatLstEntry   endOfList, 0

//######################################################
// Board sRSRC
//######################################################
.global _sRsrc_Board
.global _sRsrc_CardDisk

// all entries MUST be ascending numerical order of values of the first parameter to OSLstEntry
// failure to do so will cause it to not work at minimum and often crash 
_sRsrc_Board:    
    OSLstEntry  sRsrcType, _BoardType       // offset to board descriptor 
    OSLstEntry  sRsrcName, _BoardName       // offset to name of board 
    OSLstEntry  sRsrcIcon, _BoardIcon       // offset to icon 
    DatLstEntry boardId, drHwboardID        // board ID (assigned by DTS)
    OSLstEntry  pRAMInitData, _BoardDefPRAM
    .long OSLstEntry_primaryInit            // _sblockPriInit
    OSLstEntry  vendorInfo, _VendorInfo     // offset to vendor info 
    EndLstEntry

// required values for board resources
_BoardType:    
    .short catBoard            
    .short typeBoard
    .short 0
    .short 0
    
_BoardName:     
    .asciz "TestStorageCard V1.0" 
    .align 2

// slot PRPAM init when card is newly inserted / boardid mismatch with slot pram content 
_BoardDefPRAM:
    .long 12    // block size
    .word 0     // reserved for board ID
    .byte 0     // our data bytes follow (6)
    .byte 0
    .byte 0
    .byte 0
    .byte 0
    .byte 0

// 32x32 1BPP board icon shows in Monitors for video cards, don't know anywhere else.
_BoardIcon: 
    .byte  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
    .byte  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
    .byte  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
    .byte  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
    .byte  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x7F, 0xFF, 0xFF, 0xFE
    .byte  0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF
    .byte  0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF
    .byte  0x7F, 0xFF, 0xFF, 0xFE, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
        
//######################################################
// Vendor Info
//######################################################
_VendorInfo:    
    OSLstEntry vendorId,_VendorId      
    OSLstEntry revLevel,_RevLevel     
    OSLstEntry partNum,_PartNum      
    OSLstEntry date,_Date              
    EndLstEntry
    
_VendorId: .asciz "Vendor string" // vendor name 
_RevLevel: .asciz "Version string"
_PartNum:  .asciz "Card V1.0" 
_Date:     .asciz "Date string"

.align 2
// device resource. again, all entries must be in ascending order.
_sRsrc_CardDisk:    
    OSLstEntry  sRsrcType,_CardType       // type 
    OSLstEntry  sRsrcName,_CardName       // driver name string
    OSLstEntry  sRsrcDrvrDir,_DrvrDir     // driver directory
    .long OSLstEntry_bootRec              // bootrec required to be bootable, requires bit fOpenAtStart to be bootable
    DatLstEntry sRsrcFlags, 2             // 2 = fOpenAtStart
    DatLstEntry sRsrcHWDevId, 1           // 
    OSLstEntry minorBaseOS, _MinorBase    // offset to address space
	OSLstEntry minorLength, _MinorLength  // length of card address space
    EndLstEntry                           // end of list

.align 2
_CardType:    
    .short        catProto              
    .short        typeCustomDisk 
    .short        drSwApple               
    .short        drHwCARD          

_CardName:    
    .asciz        "Disk_CARD"      

.align 2
    _MinorBase:	 	.long	defMinorBase     
    _MinorLength: 	.long	defMinorLength   
//######################################################
// Driver directory 
//######################################################
.global _DrvrDir                         // for linker

_DrvrDir:    
    .long OSLstEntry_sMacOS68020         // _sblockDrvr        
    EndLstEntry 

//######################################################
// Monolithic C Code SBlock
//######################################################
// driver, primary, secondary sblocks; overlapping eachother
// all can use common code stored within blocks' overlap area

.global _sblockPriInit
.global _sblockBootRec
.global _sblockDrvr

.section .text.CBlock
//######################################################
// Driver sblock
//######################################################
_sblockDrvr:
    .long  _DrvrBlock_End-.               // block size     

    #include "drvr/Drvr.s"                // driver body                

//######################################################
// Primary Init sBlock
//######################################################
.section .text.InitBlocks

_sblockPriInit:

    .long   _CBlock_End-.                 // block size (from linker)
    .byte    sExec2                       // primary init
    .byte    sCPU68020                    // always 68020, even for newer 
    .short    0                           // Reserved 
    .long    PrimaryInit-.                // Offset to C entry

//######################################################
// Bootrec sBlock
//######################################################
_sblockBootRec:
    .long   _CBlock_End-.                 // block size, from linker
    .byte    sExec2                       // primary init 
    .byte    sCPU68020                    // always 68020, even for newer 
    .short    0                           // Reserved 
    .long    BootRec-.                    // Offset to C entry 

// Here shared C functions are inserted by linker followed by _DrvrBlock_End 
// Then main code for PrimaryInit and SecondaryInit, finally _CBlock_End

//######################################################
// Declaration ROM structure at end of ROM
//######################################################
.section .declstruct

DeclROMDir:
    .long RsrcDirOffset                   // offset to srcrddir supplied by linker 
DeclROMCRC:
    .long ROMSize                         // supplied by linker script 
    .long 0                               // crc filled in by script
    .byte 1                               // Revision Level 
    .byte appleFormat                     // Apple Format 
    .long testPattern                     // magic TestPattern 
    .byte 0                               // reserved 
    .byte 0x0F                            // byte lane 1,2,3,4 : dynamic bus sizing 
DeclRomEnd:
     .end
