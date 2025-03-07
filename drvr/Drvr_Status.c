#include "Drvr.h"

// status routine for the driver
OSErr DrvrStatus(CntrlParamPtr pb, AuxDCEPtr dce) {
    DRIVER_COMMON_SETUP;
  
    switch (pb->csCode) {
        case drvStat_DrvSts: 
            // Copy size of original DrvSts struct (22) as that is the size of csparam. crash otherwise.
            // struct DrvSts2 is a superset of DrvSts. 
            BlockMove(&(globs->drvsts), &pb->csParam, sizeof(DrvSts));
            breakReturn(noErr);  
        default: // always return statusErr for unknown 
            breakReturn(statusErr);
    }

    DRIVER_RETURN;
}
