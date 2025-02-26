/* SOURCE: ROMDefs.a from Universal Interfaces 
definitions for constructing a DeclROM */

appleFormat                         =         1                    /* Format of Declaration Data (IEEE will assign real value) */
romRevision                         =         1                    /* Revision of Declaration Data Format */
romRevRange                         =         9                    /* Revision of Declaration Data Format [1..9] */
testPattern                         =         1519594439            /* FHeader long word test pattern */
sCodeRev                         =         2                    /* Revision of code (For sExec) */
sExec2                             =         2
sCPU68000                         =         1                    /* CPU type = 68000 */
sCPU68020                         =         2                    /* CPU type = 68020 */
sCPU68030                         =         3                    /* CPU type = 68030 */
sCPU68040                         =         4                    /* CPU type = 68040 */
sMacOS68000                         =         1                    /* Mac OS, CPU type = 68000 */
sMacOS68020                         =         2                    /* Mac OS, CPU type = 68020 */
sMacOS68030                         =         3                    /* Mac OS, CPU type = 68030 */
sMacOS68040                         =         4                    /* Mac OS, CPU type = 68040 */
board                             =         0                    /* Board sResource - Required on all boards */
endOfList                         =         255                    /* End of list */

/* bits !!*/                                                        /*  sResource flags for sRsrc_Flags  */
fOpenAtStart                     =         1                    /*  set => open the driver at start time, else do not  */
f32BitMode                         =         2                    /*  set => a 32-bit address will be put into dctlDevBase (IM Devices 2-54)  */

/* values */
sRsrcType                         =         1                    /* Type of sResource */
sRsrcName                         =         2                    /* Name of sResource */
sRsrcIcon                         =         3                    /* Icon */
sRsrcDrvrDir                     =         4                    /* Driver directory */
sRsrcLoadDir                     =         5                    /* Load directory */
sRsrcBootRec                     =         6                    /* sBoot record */
sRsrcFlags                         =         7                    /* sResource Flags */
sRsrcHWDevId                     =         8                    /* Hardware Device Id */
minorBaseOS                         =         10                    /* Offset to base of sResource in minor space. */
minorLength                         =         11                    /* Length of sResource�s address space in standard slot space. */
majorBaseOS                         =         12                    /* Offset to base of sResource in Major space. */
majorLength                         =         13                    /* Length of sResource in super slot space. */
sRsrcTest                         =         14                    /* sBlock diagnostic code */
sRsrccicn                         =         15                    /* Color icon */
sRsrcicl8                         =         16                    /* 8-bit (indexed) icon */
sRsrcicl4                         =         17                    /* 4-bit (indexed) icon */
sDRVRDir                         =         16                    /* sDriver directory */
sGammaDir                         =         64                    /* sGamma directory */
sRsrcVidNames                     =         65                    /* Video mode name directory */

/*   =======================================================================   */
/*   sResource types                                                           */
/*   =======================================================================   */

catBoard                         =         0x0001                /* Category for board types. */
catTest                             =         0x0002                /* Category for test types -- not used much. */
catDisplay                         =         0x0003                /* Category for display (video) cards. */
catNetwork                         =         0x0004                /* Category for Networking cards. */
catScanner                         =         0x0008                /* scanners bring in data somehow */
catCPU                             =         0x000A
catIntBus                         =         0x000C
catProto                         =         0x0011
catDock                             =         0x0020                /* <Type> */
typeBoard                         =         0x0000
typeApple                         =         0x0001
typeVideo                         =         0x0001
typeEtherNet                     =         0x0001
typeStation                         =         0x0001
typeDesk                         =         0x0002
typeTravel                         =         0x0003
typeDSP                             =         0x0004
typeXPT                             =         0x000B
typeSIM                             =         0x000C
typeDebugger                     =         0x0100
type68000                         =         0x0002
type68020                         =         0x0003
type68030                         =         0x0004
type68040                         =         0x0005
type601                             =         0x0025
type603                             =         0x002E
typeAppleII                         =         0x0015                /* Driver Interface : <id.SW> */
drSwMacCPU                         =         0
drSwApple                         =         1                    /* To ask for or define an Apple-compatible SW device. */

boardId                             =         32                    /* Board Id */
pRAMInitData                     =         33                    /* sPRAM init data */
primaryInit                         =         34                    /* Primary init record */
timeOutConst                     =         35                    /* Time out constant */
vendorInfo                         =         36                    /* Vendor information List. See Vendor List, below */
boardFlags                         =         37                    /* Board Flags */
secondaryInit                     =         38                    /* Secondary init record/code */
                                                            /*  The following Id's are associated with all CPU sResources. */
MajRAMSp                         =         129                    /* ID of Major RAM space. */
MinROMSp                         =         130                    /* ID of Minor ROM space. */
vendorId                         =         1                    /* Vendor Id */
serialNum                         =         2                    /* Serial number */
revLevel                         =         3                    /* Revision level */
partNum                             =         4                    /* Part number */
date                             =         5                    /* Last revision date of the card */


/* ============================================================= */
/*  Offset List Entry */
/* ============================================================= */
.macro OSLstEntry entry_type, label
    .long (\entry_type<<24) + ((\label-.) & 0xffffff)
.endm

/* ============================================================= */
/*  Data List Entry */
/* ============================================================= */
.macro DatLstEntry entry_type, word_data
    .long (\entry_type<<24) + (\word_data & 0xffff)
.endm

/* Terminator for sResource lists */
.macro EndLstEntry
    DatLstEntry endOfList, 0 
.endm
