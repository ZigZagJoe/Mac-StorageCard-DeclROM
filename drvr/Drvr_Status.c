#include "Drvr.h"

OSErr DrvrStatus(CntrlParamPtr pb, AuxDCEPtr dce) {
    OSErr ret;

    GlobalHdl globsHdl = (GlobalHdl)dce->dCtlStorage;
    if (!dce->dCtlStorage || !(*globsHdl)) { ret = statusErr; RETURN_FROM_DRIVER; } // die HARD, should never happen

    GlobalPtr globs = *globsHdl;
  
    switch (pb->csCode) {
        case drvStat_DrvSts: 
            // only copy size of original DrvSts struct (22) as that is the size of csparam. crash otherwise.
            BlockMove(&(globs->drvsts), &pb->csParam, sizeof(DrvSts));
            breakReturn(noErr);  
        default: // always return statusErr for unknown 
            breakReturn(statusErr);
    }

    RETURN_FROM_DRIVER;
}
