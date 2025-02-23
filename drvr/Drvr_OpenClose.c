#include "Drvr.h"

OSErr DrvrOpen(IOParamPtr pb, AuxDCEPtr dce) {
    OSErr ret = noErr;

    // construct 32/24 bit address for registers in format 0xFss00000, safe in both modes 
    // if your device is addressing is compatible with this anyways! 
    // otherwise you'll need to SwapMMUMode and use only a 32 bit safe addresses, 
    // stripaddress what the OS hands you, and all sorts of fun stuff.
    
    Ptr a32 = (Ptr) (0xF0000000 | ((UInt32)dce->dCtlSlot << 24) | ((UInt32)dce->dCtlSlot << 20));
    
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
    globs->devBase32 = a32; // use this for IO later.  For some reason, the DCE devbase is empty, so we made our own
 
    /* do your hardware setup here */    
        
    if (ret != noErr)  { // a major hardwaree issue occurred, exit with error
        DisposeHandle(dce->dCtlStorage);
        dce->dCtlStorage = nil;
        return openErr;
    }    
    
    globs->sizeLBA = 10000; // 512 byte sectors! 
  
    // find a free drive number
    int myDrvNum = 1;
    DrvQElPtr dq;
    for(dq = (DrvQElPtr)(GetDrvQHdr())->qHead; dq; dq = (DrvQElPtr)dq->qLink) {
        if (dq->dQDrive >= myDrvNum)
            myDrvNum = dq->dQDrive + 1;
    }

    globs->myDrvNum = myDrvNum;

    // make a pointer for ease of writing the below piece
    DrvSts2 *drvSts = &(globs->drvsts);

    // defined on IM:Files 2-85
    drvSts->writeProt = 0;  // bit 7 = 1 if volume is locked 
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
    // if it's not valid HFS

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
        } else // traverse
            dq = (DrvQElPtr)(dq->qLink);
    }
};

// only called by A/UX and Mode32 
// a lot of the floppy drivers are just unclosable and report closErr too
OSErr DrvrClose(IOParamPtr pb, AuxDCEPtr dce) {
    OSErr ret = noErr;

    if (dce->dCtlStorage != nil) { //  NetBoot.c says this shouldn't be called with null storage, but check anyways
        GlobalHdl globsHdl = (GlobalHdl)dce->dCtlStorage;
        GlobalPtr globs = (GlobalPtr)*globsHdl;

        /* do any hardware shutdown needed. */

        // find and remove any volumes associated with this driver
        RemoveDrvrVolumes(globs->drvrRefNum);     
        
        DisposeHandle(dce->dCtlStorage);
        dce->dCtlStorage = nil;
    }

    return ret;
}
