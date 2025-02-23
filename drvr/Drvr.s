 // driver header block, as defined in IM 2 p188
 
 _DrvrStart: 
    .word          0x5f00                  // 0x5f00: dNeedGoodByeMask, dCtlEnableMask, dStatEnableMask, dReadEnableMask, dWritEnableMask, dNeedLockMask [Devices.a]
    .word       0                          // ticks between periodic (accRun)
    .word       0                          // DA event mask
    .word       0                          // menu id
    | directly call the C code 
    .word        DrvrOpen-_DrvrStart       // open routine 
    .word        DrvrPrime-_DrvrStart      // prime (IO routine)
    .word        DrvrCtl-_DrvrStart        // control 
    .word        DrvrStatus-_DrvrStart     // status 
    .word        DrvrClose-_DrvrStart      // close 
    
_DrvrName:
    .ascii driverNamePascal
_DrvrName_End:    
    .align 2
    .word 0                                // version number
    