#ifndef CONFIG_H
#define CONFIG_H

// MUST ONLY CONTAIN DEFINES
// it is shared with assembly

//////////////////////////////////////////////
// BOARD CONSTS
//////////////////////////////////////////////

/* DECLROM CONFIG */
// board ID, from srsrc type
#define drHwCARD        0xCADD 
#define drHwboardID   0xABBC

// board sResource (<128) 
#define sRsrc_Board     1            
#define sRsrc_CardDisk    0xDD

// not defined anywhere how types need to work, doesn't seem to matter what it is
#define typeCustomDisk  0xDDDD
#define driverNamePascal "\x10.Disk_CARD"

// DC&D III, pg 196
#define defMinorBase    0             // beginning 
#define defMinorLength  0x20000       // 128 KiB 

#endif