#include "Drvr.h"

// storage routine for the driver
// if you implement multiple volumes per driver, you will need to translate the values
// provided by mac os appropriately to your device(s) (ie. partitions -> add offsets)

OSErr DrvrPrime(IOParamPtr pb, AuxDCEPtr dce) {
    
    OSErr ret = noErr;
    GlobalHdl globsHdl = (GlobalHdl)dce->dCtlStorage;

    if (!dce->dCtlStorage || !(*globsHdl)) { // should never happen
        ret = nsDrvErr; // return a fatal error (offlineErr is nonfatal and might get retried)
        RETURN_FROM_DRIVER;
    }    

    GlobalPtr globs = *globsHdl;

    // cast to Uint32, and do no signed math beforehand with these values
    // this will make us 4GB safe under 7.5+
    uint32_t byteOffsetAbs = 0;
    uint32_t byteCount = pb->ioReqCount; // always a multiple of 512

    switch(pb->ioPosMode & 0x000F) { // figure the actual offset according to the mode
        case fsAtMark:
            byteOffsetAbs = dce->dCtlPosition;
            break;
        case fsFromStart:
            byteOffsetAbs = pb->ioPosOffset;
            break;
        case fsFromMark: 
            byteOffsetAbs = (uint32_t)dce->dCtlPosition + (uint32_t)pb->ioPosOffset;
            break;
    }

    if (byteCount % SECTOR_SZ) {  // per Blue Book Vol2 pg 473 (Edisk Prime), much useful information here!
        // Access is not a full sector size
        ret = paramErr;
        goto exitPrime;
    }
    
    uint32_t addressLBA = byteOffsetAbs / SECTOR_SZ;
    uint16_t sectorCount = byteCount / SECTOR_SZ;  
    
    if ((addressLBA + sectorCount - 1 /* zero indexed sector */) > globs->sizeLBA) {
        // Out of range sector access
        ret = paramErr;
        goto exitPrime;
    }
    
    Ptr buffAddr = pb->ioBuffer;
    uint32_t bytesActual = 0; // put how much data you transferred in here
    
    // do the actual IO here (Synchronous)
    // You can also do asynch calls, but they're more complicated: you'll need a way to re-enter the driver (ie. interrupt) also need to support killIO calls
    if((pb->ioTrap & 0x00ff) == aRdCmd) {
        if(!(pb->ioPosMode & 0x40)) { // bit 6 indicates verify operation, system 6 mostly uses this?
            // read byteCount from byteOffsetAbs on media into buffAddr
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
        // if read only disk, instead return wPrErr!
        bytesActual = byteCount;
        ret = noErr; // data written successfully!
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
    RETURN_FROM_DRIVER;
}

