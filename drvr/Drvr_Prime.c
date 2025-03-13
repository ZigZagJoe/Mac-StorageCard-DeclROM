#include "Drvr.h"

// storage routine for the driver
// if you implement multiple volumes per driver, you will need to translate the values
// provided by mac os appropriately to your device(s) (ie. partitions -> add offsets)

OSErr DrvrPrime(IOParamPtr pb, AuxDCEPtr dce) {
    
    DRIVER_COMMON_SETUP;

    // per Technote FL 24; only use dCtlPosition as Device Manager sets it up for us, considering offset, if needed
    // cast to Uint32, and do no signed math before doing so
    // this will make safe to present volumes up to 4GB in size under System 7.5+
    uint32_t byteOffsetAbs = dce->dCtlPosition;
    uint32_t byteCount = pb->ioReqCount; // always a multiple of 512
    
    uint32_t addressLBA = byteOffsetAbs / SECTOR_SZ;
    uint16_t sectorCount = byteCount / SECTOR_SZ;  
    
    Ptr buffAddr = pb->ioBuffer;
    
    // perform sanity checks
    if (byteCount % SECTOR_SZ) {  // per Blue Book Vol2 pg 473 (Edisk Prime), much useful information here!
        ret = paramErr; // Access wasn't a full sector; very much invalid
        goto exitPrime;
    }

    if ((addressLBA + sectorCount - 1 /* zero indexed sector */) > globs->sizeLBA) {
        ret = paramErr; // Out of range sector access
        goto exitPrime;
    }
    
    uint32_t bytesActual = 0; // put how much data you transferred in here
 
    // do the actual IO here (Synchronous)
    // You can also do asynch calls, but they're more complicated: you'll need a way to re-enter the driver (ie. interrupt) also need to support killIO calls
    if((pb->ioTrap & 0x00ff) == aRdCmd) {
        if(!(pb->ioPosMode & 0x40)) { // bit 6 indicates verify operation, system 6 mostly uses this?
            // read byteCount from byteOffsetAbs on media into buffAddr
            
            BlockMove(((Ptr)*globs->ramDiskHdl) + byteOffsetAbs, buffAddr, byteCount); // ramdisk example read
            
            bytesActual = byteCount;
            ret = noErr; // data read successfully!
        } else {
            // VERIFY byteCount from byteOffsetAbs on media MATCHES buffAddr contents
            // return dataVerErr if not matching. uncommon, but sys6 does use this mode
            bytesActual = byteCount;
            ret = noErr; // we silently return OK without verifying data.
        }
    } else if((pb->ioTrap & 0x00ff) == aWrCmd) {
        // write byteCount from buffAddr to byteOffsetAbs on media 

        BlockMove(buffAddr, ((Ptr)*globs->ramDiskHdl) + byteOffsetAbs, byteCount); // ramdisk example write
        
        bytesActual = byteCount;
        ret = noErr; // data written successfully!
        // if read only disk, instead return wPrErr!
    }

    // SuperMario dump: EDisk, Newage, SonyRWT update these fields always even if an error has occured. SonyIOP does not
    if (bytesActual) { // update fields and return partial data if anything was done
        pb->ioActCount = bytesActual; // actual data read
        dce->dCtlPosition = byteOffsetAbs + pb->ioActCount; // current r/w position
        pb->ioPosOffset = dce->dCtlPosition; 
    } 

    if (bytesActual != byteCount) // something went wrong; return an error.
        ret = ioErr; // how it is handled by MacOS is not defined but do not return partial read/write data without an error code

exitPrime:
    DRIVER_RETURN;
}

