#ifndef CONFIG_H
#define CONFIG_H

// MUST ONLY CONTAIN DEFINES
// it is shared with assembly

#define SECTOR_SZ 512

// ramdisk example size
#define DISK_SZ_MB 8

// calculated size in sectors and bytes
#define DISK_SZ_BYTES (DISK_SZ_MB * 1024 * 1024)
#define DISK_SZ_SECT (DISK_SZ_MB * SECTOR_SZ * 4)

//////////////////////////////////////////////
// BOARD CONSTS
//////////////////////////////////////////////

// board ID, from srsrc type
// arbitrary
#define drHwCARD        1234 
#define drHwboardID     1

// board sResource (<128) 
#define sRsrc_Board     1            
#define sRsrc_CardDisk  0xDD

// arbitrary custom disk type
#define typeCustomDisk  0xD
#define driverNamePascal "\x10.Disk_CARD"

// used to construct dCtlDevBase
// DC&D III, pg 196
#define defMinorBase    0             // beginning 
#define defMinorLength  0x20000       // 128 KiB 

#endif