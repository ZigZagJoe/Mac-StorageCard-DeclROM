#ifndef __MACDRIVECALLS__
#define __MACDRIVECALLS__

// fmtLstCode         Get Format List status call
#define drvStat_fmtLst 6

// StsCode Drive Status call
#define drvStat_DrvSts 8 

// FLOPPY ONLY mfmStsCode 'Get MFM status' status code
#define drvStat_mfmStsCode 10

// EDISK ONLY SizeCode Drive Size Status call
#define drvStat_Size     0x4544    

/* CONTROL */
// (dNeedGoodbye) goodbye application heap being reinitialized, also called at shutdown
#define drvCtl_goodbye      -1 

// KillCode, terminate async IO
// must return via RTS, never IOdone. IM II P194
#define drvCtl_KillIO        1

// verifyCC 'verify media' control code
// The verify function requests a read verification of the data on the hard disk drive media
// Per IM 4: Verifies disk formatting (structures, etc)
#define drvCtl_Verify        5

// formatCC  'format media' control code
// The format function initializes the hard drive for use by the operating system
#define drvCtl_Format        6

// EjectCode 'Eject media' control call
// The eject media function prepares and initiates an eject operation from the specified drive. returns noErr if the logical drive number is valid
// if controlError is returned, disk will be re-inserted at next vSync
#define drvCtl_Eject         7

// tagBufCC  'set tag buffer' control code
// Set a new tag buffer (CSParam != 0) else revert to the one in the driver
#define drvCtl_tagBufCC        8

// tCacheCC  'track cache' control. floppy only
#define drvCtl_tCacheCC        9

// iconIDCC 'get icon id' control code. depreciated?
#define drvCtl_iconIDCC      20 

// iconCC Physical Drive Icon control call (ICN# data). 
//The return drive icon function returns a pointer to the device icon and the device name string. 
#define drvCtl_iconCC         21 

// iconLogCC Disk Media Icon control call (ICN# data). 
// The return media icon function returns a pointer to the device icon and the name string. 
#define drvCtl_iconLogCC    22 

// infoCC 'get drive info' code
// The return drive characteristics function returns information about the characteristics of the specified drive as defined in Inside Macintosh, Volume V.
// LONG expected, see IM 5 V-471
#define drvCtl_infoCC        23   

// RetriesCC 'Enable/Disable Retries' code
#define drvCtl_RetriesCC    17232

// GetRawDataCC     'get raw track data' 
#define drvCtl_GetRawDataCC 18244    

// accRun Time for periodic action (dNeedTime)
// allows the driver time to perform periodic operations. Chapter 6 of Inside Macintosh, Volume II
#define drvCtl_accRun 65

#endif