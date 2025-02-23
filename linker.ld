OUTPUT_FORMAT("binary");
ENTRY(DeclROMDir);

MEMORY {
  ROM (rx)  : ORIGIN = 0, LENGTH = 32K
}

/* sRecord types */
primaryInit = 34;
secondaryInit = 38;
sRsrcBootRec = 6;
sMacOS68020 = 2;

SECTIONS {
  /DISCARD/ : { 
    *(.comment) 
  }

  .text : SUBALIGN (4) {
    /* resource dir & sRSRCs */
    *declrom.o (.text)
 
    /* nested sBlocks & c code */ 
    *declrom.o (.text.CBlock)       /* drvr-only code */
    *Drvr_OpenClose.o (*)
    *Drvr_Prime.o (*)
    *Drvr_Control.o (*)
    *Drvr_Status.o (*)  
    *declrom.o (.text.InitBlocks)   
    /* shared code between driver and inits can go here */
    PROVIDE(_DrvrBlock_End = .);   /* init-only code */
    *PrimaryInit.o (*)
    *BootRec.o (*)
    PROVIDE(_CBlock_End = .);

    /* perform math to fill out OSLstEntry fields */
    PROVIDE(OSLstEntry_primaryInit =   ((primaryInit << 24)   | ((_sblockPriInit - _sRsrc_Board - 20) & 0xFFFFFF))); /* hardcoded directory offset 16 */
    PROVIDE(OSLstEntry_bootRec =       ((sRsrcBootRec << 24) | ((_sblockBootRec - _sRsrc_CardDisk - 12) & 0xFFFFFF))); /* hardcoded directory offset 12 */
    PROVIDE(OSLstEntry_sMacOS68020 =   ((sMacOS68020 << 24)   | ((_sblockDrvr - _DrvrDir) & 0xFFFFFF)));

    /* declrom structure */
    *declrom.o (.declstruct)
    PROVIDE(ROMSize = .);
    PROVIDE(RsrcDirOffset = ((0 - .) + 20 /* size of struct */) & 0xFFFFFF);
  } > ROM

  /DISCARD/ : { *(*) }
}
