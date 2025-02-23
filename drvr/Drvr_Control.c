#include "Drvr.h"

const unsigned char DiskIcon[256+1+7] = { 
    /* icon */
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x7F, 0xFF, 0xFF, 0xFE,
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
    0x7F, 0xFF, 0xFF, 0xFE, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    /* mask */
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x7F, 0xFF, 0xFF, 0xFE,
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
    0x7F, 0xFF, 0xFF, 0xFE, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    /* pascal string for physical location of volume */
    4, // length
    'C','a','r','d' // location string
}; 

// control routine
OSErr DrvrCtl(CntrlParamPtr pb, AuxDCEPtr dce) {
    
    OSErr ret;

    GlobalHdl globsHdl = (GlobalHdl)dce->dCtlStorage;
    if (!dce->dCtlStorage || !(*globsHdl)) { ret = controlErr; RETURN_FROM_DRIVER; } // die HARD, should never happen
    GlobalPtr globs = (GlobalPtr)*globsHdl;
    
    switch (pb->csCode) {
        case drvCtl_Format: // prepare media for use, if required
        case drvCtl_Verify: // verify format
            breakReturn(noErr); // do nothing; HFS will write the sectors it needs to only rather than zero the disk  
        case drvCtl_infoCC:                
            *((uint32_t*)&pb->csParam) = 0b011000000001; // Unspecified drive type, internal, SCSI, fixed, primary drive. IM 5 p471, PB 150 Devnote
            breakReturn(noErr);  
        case drvCtl_iconCC:    // return drive icon
        case drvCtl_iconLogCC: // return drive media icon
            *((Ptr*)&(pb->csParam)) = (Ptr)&DiskIcon;
            breakReturn(noErr);  
        case drvCtl_goodbye: 
            // occurs at shutdown and other times the driver could go away
            // application heap reinitialization is another, sys6 will do that at finder load
            // but as we're a locked driver we are immune to heap reinitialization
            // Since we can't differentiate the two events: batten down the hatches! 
            // make ready for shutdown: flush and disable caches, as power could go away after this
            // More writes can still occur so don't actually make irreversible changes though.
            breakReturn(noErr);  
        case drvCtl_Eject: // happens at shutdown (7.5+) or if not bootable. 
            // either way drive will be unmounted until an insert event is posted
            breakReturn(noErr);
        default: //  return controlErr for unimplemented codes
            breakReturn(controlErr);
    }

    RETURN_FROM_DRIVER;
}
