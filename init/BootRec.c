#include "Drvr.h"

/* conditions and timing for execution
sresource must have flags = fOpenAtBoot bit 1 (value 2) set
XPRAM $78 must be set to 00000SRR where S is slot and RR is resource ID

if pram is per above:
    bootrec will be called at early boot (just after PrimaryInit) at which point
    it can install the driver and be bootable. (it will still be called at secondaryInit time too)
    Quickdraw is available at this point, as is keyboard/mouse

If PRAM is not set
    BootRec will be called after secondary init
    it is responsible for loading the device driver
*/

// Don't call any functions defined in the Drvr files! They aren't going to be in the SBlock copied to RAM by slot manager.
// All functions shared must be either inline or defined in an object linked after the .text.InitBlocks section

UInt32 BootRec(SEBlock* seblock) {
    seblock->seStatus = 1; // code was executed
    //seblock->seBootState  0 = early boot, 1 = secondaryInit
 
    OSErr ret;
    SlotDevParam pb;

    char name[] = driverNamePascal; 

    // directly lifted from Patches/VideoPatch.a #1006-1013
    // See DC&D 3rd Pg 195 (1st ed, Pg 9-7)
    pb.ioSlot = seblock->seSlot;
    pb.ioID = sRsrc_CardDisk;
    pb.ioNamePtr = (StringPtr)&driverNamePascal;
    pb.ioSFlags = 0;
    pb.ioSPermssn = 0;
    pb.ioSMix = nil; // reserved for use by driver, can pass vars here to the driver

    ret = PBHOpen((HParamBlockRec*)&pb, false /* not async */);

     // return refnum of driver on success
    if (ret == noErr) {
        // loop through drives, looking for drives handled by our driver. post events on them if so
        DrvQElPtr dq;
        for(dq = (DrvQElPtr)(GetDrvQHdr())->qHead; dq; dq = (DrvQElPtr)dq->qLink) {
            if (dq->dQRefNum == pb.ioSRefNum) {
                // notify mac OS of the new drive and cause it to be mounted
                // this is done both times BootRec is called as ROM might have ejected the drive if it didn't have a valid System folder
                PostEvent(diskInsertEvt, dq->dQDrive /* drive # */); // thank you elliotnunn
            }
        }
    
        return pb.ioSRefNum;
    } 

    return 0;
}

