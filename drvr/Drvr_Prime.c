#include "Drvr.h"

OSErr DrvrPrime(IOParamPtr pb, AuxDCEPtr dce) {
    
    OSErr ret = noErr;
    GlobalHdl globsHdl = (GlobalHdl)dce->dCtlStorage;

    if (!dce->dCtlStorage || !(*globsHdl)) { // should never happen
        ret = nsDrvErr; // return a fatal error (offlineErr is nonfatal and might get retried)
        RETURN_FROM_DRIVER;
    }    

    GlobalPtr globs = *globsHdl;

    // as we cast to Uint32, and do no signed math beforehand so we are 4GB safe under 7.5
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
    
    Ptr buffAddr = pb->ioBuffer;
    uint32_t bytesActual = 0; // put how much data you transferred in here
    
    // do the actual IO here (Synchronous)
    // You can also do asynch calls, but you'll need a way to re-enter the driver (ie. interrupt) also need to support killIO calls
    if((pb->ioTrap & 0x00ff) == aRdCmd) {
        if(!(pb->ioPosMode & 0x40)) { // bit 6 indicates verify operation, system 6 mostly uses this?
            // read byteCount from byteOffsetAbs on media into buffAddr
        } else {
            // verify byteCount from byteOffsetAbs on media MATCHES buffAddr contents, return dataVerErr if not
            // uncommon, but sys6 does seem to use this. You can silently return OK, too...
        }
    } else if((pb->ioTrap & 0x00ff) == aWrCmd) {
        // write byteCount from buffAddr to byteOffsetAbs on media 
    }

    // EDisk, Newage, SonyRWT update these always, even if an error has occured. SonyIOP does not
    if (bytesActual) { // update fields and return partial data if anything was done
        pb->ioActCount = bytesActual; // actual data read
        dce->dCtlPosition = byteOffsetAbs + pb->ioActCount; // current r/w position
        pb->ioPosOffset = dce->dCtlPosition; 
    } 

    if (bytesActual != byteCount)
        ret = ioErr; // something went wrong; return an error; how it is handled is not quite defined

    RETURN_FROM_DRIVER;
}

