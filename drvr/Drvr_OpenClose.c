#include "Drvr.h"

OSErr DrvrOpen(IOParamPtr pb, AuxDCEPtr dce) {
    OSErr ret = noErr;

    if (dce->dCtlStorage != nil) { // should never happen, double driver open
        return noErr; // ???
    }

    // initialize private storage, zeroed
    ReserveMemSys(sizeof(Global));
    dce->dCtlStorage = NewHandleSysClear(sizeof(Global));
    
    if (dce->dCtlStorage == nil) {
        // failed alloc storage!
        return openErr;
    }

    HLock(dce->dCtlStorage);
    GlobalHdl globsHdl = (GlobalHdl)dce->dCtlStorage;
    GlobalPtr globs = (GlobalPtr)*globsHdl;

    globs->drvrRefNum = dce->dCtlRefNum;
	
	// For some reason, the DCE devbase is typically empty, so we make our own
    // constructs a 32/24 bit address for registers in format 0xFss00000, safe in both modes 
    // if your device addressing is compatible with this anyways! if it does not decode A20-A23, you will
    // be limited to a 1MB address space repeating through the 16MB nubus pseduoslot space and will be OK.
    // otherwise you'll need to SwapMMUMode and use only a 32 bit safe addresses, 
    // stripaddress what the OS hands you, and all sorts of fun stuff.

    Ptr a32 = (Ptr) (0xF0000000 | ((UInt32)dce->dCtlSlot << 24) | ((UInt32)dce->dCtlSlot << 20));
    globs->devBase32 = a32; // use this for IO later.  
 
    /* do your hardware setup here, set ret on some sort of failure */    
        
    if (ret != noErr)  { // a major hardware issue occurred, exit with error
        DisposeHandle(dce->dCtlStorage);
        dce->dCtlStorage = nil;
        return openErr;
    }    
 
    // find a free drive number
    int myDrvNum = 1;
    DrvQElPtr dq;

    for(dq = (DrvQElPtr)(GetDrvQHdr())->qHead; dq; dq = (DrvQElPtr)dq->qLink) {
        if (dq->dQDrive >= myDrvNum)
            myDrvNum = dq->dQDrive + 1;
    }

    globs->myDrvNum = myDrvNum;
    globs->sizeLBA = 10000; // size in 512 byte sectors!
  
    // make a pointer for ease of writing the below piece
    DrvSts2 *drvSts = &(globs->drvsts);

    // defined on IM:Files 2-85
    drvSts->writeProt = 0;  // bit 7 = 1, value 0x80, if volume is locked 
    drvSts->diskInPlace = 8; // nonejectable disk, 
    drvSts->installed = 1; // drive installed
    drvSts->qType = 1; // 1 for HD20, enables S1
    drvSts->dQDrive = myDrvNum; // drive number 
    drvSts->dQRefNum = globs->drvrRefNum; // driver reference number 
    drvSts->dQFSID = 0; // HFS

    // drive size in sectors
    drvSts->driveSize = (globs->sizeLBA & 0xFFFF); 
    drvSts->driveS1 = (globs->sizeLBA >> 16);

    // Call AddDrive
    // drive num in upper word, drvr RefNum in low word
    { // probably a cleaner way to do this, w/e
        register uint32_t num asm("d0")  = (drvSts->dQDrive << 16) | (drvSts->dQRefNum & 0xFFFF);
        register uint32_t queEl asm("a0")  = (uint32_t)&drvSts->qLink;
        asm volatile(".word 0xA04E" : : "d" (num), "a" (queEl));
    }
 
    // BootRec will post a diskinsert event against this driver number
    // this will cause Mac OS to try to mount it and also ask to initialize
    // if it's not valid HFS. No need to call mountvol, that's short circuiting
	// how all this business is intended to work and can crash.

    // Yay! we're exposing a new volume to Mac OS. Note that Mac OS's understanding of partitions is tied
    // to SCSI drives! We're exposing a raw HFS filesystem here like a giant floppy, so there is
    // no partition map. If you want multiple volumes/partitions, you will need to implement 
    // understanding of a partition map inside your driver (and provide a way to create partitions!)
    // Then, modify your Prime, Control, and Status routines to operate on the volumes you expose seperately.
    
    return ret;
}

// remove all volumes for this drvr refnum from the drive queue
void RemoveDrvrVolumes(short refNum) {
    QHdrPtr QHead = (QHdrPtr)0x308; // lowmem
    DrvQElPtr dq = (DrvQElPtr)(QHead->qHead);

    while ((dq != (DrvQElPtr)(QHead->qTail))) {
        if (dq->dQRefNum == refNum) {
            Dequeue((QElemPtr)dq,QHead);
            DisposePtr((Ptr)dq);
            
            dq = (DrvQElPtr)(QHead->qHead); // start over again
        } else // traverse linked list
            dq = (DrvQElPtr)(dq->qLink);
    }
}

// only called by A/UX and Mode32 
// a lot of the floppy drivers are just unclosable and report closErr too
OSErr DrvrClose(IOParamPtr pb, AuxDCEPtr dce) {
    OSErr ret = noErr;

    if (dce->dCtlStorage != nil) { //  NetBoot.c says DrvrClose wouldn't be called with null storage, but check anyways
        GlobalHdl globsHdl = (GlobalHdl)dce->dCtlStorage;
        GlobalPtr globs = (GlobalPtr)*globsHdl;

        /* do any hardware shutdown needed. flush cache, etc */

        // find and remove any volumes associated with this driver
        RemoveDrvrVolumes(globs->drvrRefNum);     
        
        // free storage
        DisposeHandle(dce->dCtlStorage);
        dce->dCtlStorage = nil;
    }

    return ret;
}
