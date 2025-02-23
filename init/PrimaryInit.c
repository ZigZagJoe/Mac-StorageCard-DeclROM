#include "Drvr.h"

UInt32 PrimaryInit(SEBlock* seblock) {
    seblock->seStatus = 1;    // code was executed

    // You can do fundamental hardware sanity checking here. No QD, no mouse/keyboard, etc....
    // Not sure if a failure status here prevents Bootrec from being called, but I'd think it would
    // You can manipulate XPRAM here in order to ensure bootrec is called, if you like!
    // See Bootrec.c for details
    // Don't call any functions defined in the Drvr files! They aren't going to be in the code copied to RAM!
}

