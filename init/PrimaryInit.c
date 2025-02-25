#include "Drvr.h"

// You can do fundamental hardware sanity checking here and setup.
// Very early, no QD, no mouse/keyboard, etc....
// Don't call any functions defined in the Drvr files! They aren't going to be in the SBlock copied to RAM by slot manager.
// All functions shared must be either inline or defined in an object linked after the .text.InitBlocks section

UInt32 PrimaryInit(SEBlock* seblock) {
    seblock->seStatus = 1;    // code was executed

    // set seblock->seStatus = -1; for error occurred! will not be retried. there is a magic number that can be used to
    // defer primaryInit execution until secondaryInit time (system patches loaded) also.
    // Have not tested if a failure status here prevents Bootrec from being called, but I'd think it would

    // You can manipulate XPRAM here in order to ensure bootrec is called, if you like!
    // See Bootrec.c for details of what XPRAM must contain.

    return 0;
}

