#include "Drvr.h"

/* 
* For bootrec to run at boot time and be able to boot from *
    device sResource must have flags = fOpenAtBoot (value 2) set
    XPRAM $78 must be set to $00000SRR where S is slot # and RR is resource ID

* execution times *
if pram is set as above:
    bootrec will be called at early boot (just after PrimaryInit) at which point
    it can install the driver and be bootable. (it will still be called at secondaryInit time too)
    Quickdraw is available at this point, as is keyboard/mouse

If PRAM is not set or is set to other device:
    BootRec will be called after secondary init
    it is still responsible for loading the device driver

BootRec should call PostEvent(diskInsertEvt, num) on any drives belonging to your driver.

For non-bootable cards, you can not put a BootRec entry if you wish
    Mac OS will load the driver for you around SecondaryInit time. 
    Remember to PostEvent on your drives!
*/

// Don't call any functions defined in the Drvr files! They aren't going to be in the SBlock copied to RAM by slot manager.
// All functions shared must be either inline or defined in an object linked after the .text.InitBlocks section

UInt32 BootRec(SEBlock* seblock) {
    seblock->seStatus = 1; // code was executed
    // seblock->seBootState: 0 running at early boot, 1 running at secondaryInit time
 
    OSErr ret;
    SlotDevParam pb;

    // IM: Devices 1-19 suggests testing for space in the unit table prior to opening a driver
    // Given the particular times at which bootrec executes the system is in *dire* straits
    // if there were no entries left. there is not a need to check for space in Bootrecs.

    // directly lifted from Patches/VideoPatch.a #1006-1013
    // See DC&D 3rd Pg 195 (1st ed, Pg 9-7)
    pb.ioSlot = seblock->seSlot;
    pb.ioID = sRsrc_CardDisk;
    pb.ioNamePtr = (StringPtr)&driverNamePascal;
    pb.ioSFlags = 0;
    pb.ioSPermssn = 0;
    pb.ioSMix = nil; // reserved for use by driver, can pass vars here to the driver
    pb.ioSRefNum = 0;
    
    // Ask the toolbox to open the driver; it will search the card for the driver and open it
    ret = PBHOpenSync((HParamBlockRec*)&pb); // OpenSlot is just an alias for (PB)HOpen

    if (ret != noErr) // failed, return 0
        return 0;

    // loop through drives, looking for drives handled by our driver. post events on them if so
    DrvQElPtr dq;
    for(dq = (DrvQElPtr)(GetDrvQHdr())->qHead; dq; dq = (DrvQElPtr)dq->qLink) {
        if (dq->dQRefNum == pb.ioSRefNum) {
            // notify mac OS of the new drive and cause it to be mounted
            // this is done both times BootRec is called as ROM might have ejected the drive if it didn't have a valid System folder
            PostEvent(diskInsertEvt, dq->dQDrive /* drive # */); // thank you elliotnunn
        }
    }
    
    // bootrec should return refnum of driver on success
    pb.ioSRefNum = pb.ioSRefNum;
    return 0;
}

