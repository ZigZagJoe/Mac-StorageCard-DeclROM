#ifndef DRVR_H
#define DRVR_H

#include <Devices.h>
#include <Slots.h>
#include <Traps.h>
#include "MacDriveCalls.h"
#include "config.h"

////////////////////////////////////////////////////
/// CONSTS
////////////////////////////////////////////////////

#define diskInsertEvt     7 /* aka diskEvt, see sysequ */

////////////////////////////////////////////////////
/// STRUCTS
////////////////////////////////////////////////////

/// Global struct, allocated by driver
struct Global {
    Ptr devBase32;            // devbase (constructed) 32 bit/24 bit
    uint8_t slot;
    int16_t drvrRefNum;         // driver RefNum for this instance

    DrvSts2 drvsts;
    uint32_t sizeLBA;
    short myDrvNum; // volume num
};

typedef struct Global Global;
typedef struct Global *GlobalPtr;
typedef struct Global **GlobalHdl;

////////////////////////////////////////////////////
/// PROTOTYPES
////////////////////////////////////////////////////

//// Utilities for driver code /////

// convienence function for switches
#define breakReturn(val)  { ret = val; break; }

// return from control routines
#define RETURN_FROM_DRIVER     { if (!(pb->ioTrap & (1<<noQueueBit))) { IODone((DCtlPtr)dce, ret); } return ret; }

////  PrimaryInit ////
#pragma parameter __D0 PrimaryInit(__A0)
UInt32 PrimaryInit(SEBlock* block);

#pragma parameter __D0 BootRec(__A0)
UInt32 BootRec(SEBlock* seblock);

////  CONTROL ////
#pragma parameter __D0 DrvrCtl(__A0, __A1)
OSErr DrvrCtl(CntrlParamPtr pb, AuxDCEPtr dce);

//// OpenClose 
#pragma parameter __D0 DrvrOpen(__A0, __A1)
OSErr DrvrOpen(IOParamPtr pb, AuxDCEPtr dce) ;

#pragma parameter __D0 DrvrClose(__A0, __A1)
OSErr DrvrClose(IOParamPtr pb, AuxDCEPtr dce);

//// Prime routine
#pragma parameter __D0 DrvrPrime(__A0, __A1)
OSErr DrvrPrime(IOParamPtr pb, AuxDCEPtr dce);

//// Status
#pragma parameter __D0 DrvrStatus(__A0, __A1)
OSErr DrvrStatus(CntrlParamPtr pb, AuxDCEPtr dce);


#endif