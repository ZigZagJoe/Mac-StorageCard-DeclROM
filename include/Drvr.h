#ifndef DRVR_H
#define DRVR_H

#include <Devices.h>
#include <Slots.h>
#include "MacDriveCalls.h"
#include "config.h"

////////////////////////////////////////////////////
/// CONSTS
////////////////////////////////////////////////////

#define diskInsertEvt diskEvt /* older, more verbose name */

////////////////////////////////////////////////////
/// STRUCTS
////////////////////////////////////////////////////

/// Global struct, allocated by driver
struct Global {
    short myDrvNum;       // volume num
    uint32_t sizeLBA;     // size in 512 byte sectors
    DrvSts2 drvsts;       // drive info struct for status; contains drive queue element
};

typedef struct Global Global;
typedef struct Global *GlobalPtr;
typedef struct Global **GlobalHdl;

//// Utilities for driver code /////

// declare ret and globals, check globals for validity
#define DRIVER_COMMON_SETUP   OSErr ret = noErr; \
	    GlobalHdl globsHdl = (GlobalHdl)dce->dCtlStorage; \
	    if (!globsHdl || !(*globsHdl)) { ret = nsDrvErr; DRIVER_RETURN; }; \
        GlobalPtr globs = *globsHdl;

// set function ret value and break switch
#define breakReturn(val) { ret = val; break; }

// return from driver routines correctly according to mode of call
#define DRIVER_RETURN { if (!(pb->ioTrap & (1<<noQueueBit))) { IODone((DCtlPtr)dce, ret); } return ret; }

////////////////////////////////////////////////////
/// PROTOTYPES
////////////////////////////////////////////////////

////  PrimaryInit routine
#pragma parameter __D0 PrimaryInit(__A0)
UInt32 PrimaryInit(SEBlock* block);

////  BootRec routine
#pragma parameter __D0 BootRec(__A0)
UInt32 BootRec(SEBlock* seblock);

////  Control routine
#pragma parameter __D0 DrvrCtl(__A0, __A1)
OSErr DrvrCtl(CntrlParamPtr pb, AuxDCEPtr dce);

//// Open & Close routines
#pragma parameter __D0 DrvrOpen(__A0, __A1)
OSErr DrvrOpen(IOParamPtr pb, AuxDCEPtr dce);

#pragma parameter __D0 DrvrClose(__A0, __A1)
OSErr DrvrClose(IOParamPtr pb, AuxDCEPtr dce);

// OpenClose Functions
void RemoveDrvrVolumes(short refNum);

//// Prime routine
#pragma parameter __D0 DrvrPrime(__A0, __A1)
OSErr DrvrPrime(IOParamPtr pb, AuxDCEPtr dce);

//// Status routine
#pragma parameter __D0 DrvrStatus(__A0, __A1)
OSErr DrvrStatus(CntrlParamPtr pb, AuxDCEPtr dce);

#endif