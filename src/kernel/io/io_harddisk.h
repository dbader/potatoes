#ifndef __IO_HARDDISK_H_
#define __IO_HARDDISK_H_

#include "../include/types.h"


#define HDBASE 0x1F0
#define HDREG_DATA 0
#define HDREG_ERR 1
#define HDREG_COUNT 2
#define HDREG_SEC 3
#define HDREG_CYL_LOW 4
#define HDREG_CYL_HIGH 5
#define HDREG_DRIVE 6
#define HDREG_STAT 7
#define HDREG_CMD 7
#define HDALTBASE 0x3F0
#define HDALTREG_STAT 6
#define HDALTREG_ADDR 7

#define MASTERDRIVE 0xA0
#define SLAVEDRIVE 0xB0

#define HDCMD_EXEC_DRIVE_DIAG 0x90
#define HDCMD_IDENTIFY_DEVICE 0xEC
#define HDCMD_READ 0x20
#define HDCMD_WRITE 0x30
#define HDCMD_FLUSH_CACHE 0xE7

/**
 * This struct is filled by the IDENTIFY DRIVE command 
 * 
 * contains hard disk geometry data and more
 */
struct hd_info{
        uint16 config_word;
        uint16 num_cyl;
        uint16 reserved1;
        uint16 num_head;
        uint16 bytes_per_track;
        uint16 bytes_per_sector;
        uint16 sector_per_track;
        uint16 manufacturer1[3];
        uint16 serial[10];
        uint16 buffer_type;
        uint16 buffer_size; //in 512 Bytes
        uint16 num_ecc_bytes;
        uint16 firmware[4];
        uint16 type[20];
        uint16 rw_multiple_flg; //Bits 7-0
        uint16 dw_io_flg; //Bit 0
        uint16 lba_dma_flg; //Bit 9 LBA supported, Bit 8 DMA supported
        uint16 reserved2;
        uint16 timingmode_pio; //Bits 15-8
        uint16 timingmode_dma; //Bits 15-8
        uint16 reserved3;
        uint16 apparent_cyl;
        uint16 apparent_head;	
        uint16 apparent_sector_per_track;
        uint16 apparent_capacity[2];
        uint16 sectors_per_int; //Bits 7-0
        uint16 num_lba_sectors[2];
        uint16 mode_single_dma; //Bits 15-8 active mode, Bits 7-0 supported modes 
        uint16 mode_multi_dma; //Bits 15-8 active mode, Bits 7-0 supported modes
        uint16 reserved4[64];
        uint16 manufacturer2[32];
        uint16 reserved5[96];
}__attribute__((__packed__));

/**
 * address data for the hard disk packed in a struct 
 */
struct address{
        uint32 cyl;
        uint16 head;
        uint16 sector;
};
#endif /*__IO_HARDDISK_H_*/
