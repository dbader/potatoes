#ifndef __IO_HARDDISK_H_
#define __IO_HARDDISK_H_

#include "../include/types.h"

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
	uint16 flg; //Bit 9 LBA supported, Bit 8 DMA supported
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

#endif /*__IO_HARDDISK_H_*/
