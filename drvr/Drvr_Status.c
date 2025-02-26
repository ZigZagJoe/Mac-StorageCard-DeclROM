#include "Drvr.h"

// status routine for the driver
OSErr DrvrStatus(CntrlParamPtr pb, AuxDCEPtr dce) {
    OSErr ret;

    GlobalHdl globsHdl = (GlobalHdl)dce->dCtlStorage;
    if (!dce->dCtlStorage || !(*globsHdl)) { ret = nsDrvErr; RETURN_FROM_DRIVER; } // return fatal error, should never happen

    GlobalPtr globs = *globsHdl;
  
    switch (pb->csCode) {
        case drvStat_DrvSts: 
            // Copy size of original DrvSts struct (22) as that is the size of csparam. crash otherwise.
            // struct DrvSts2 is a superset of DrvSts. 
            BlockMove(&(globs->drvsts), &pb->csParam, sizeof(DrvSts));
            breakReturn(noErr);  
        default: // always return statusErr for unknown 
            breakReturn(statusErr);
    }

    RETURN_FROM_DRIVER;
}
