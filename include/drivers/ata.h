#ifndef _ATA_H
#define _ATA_H

#include <linux/types.h>

void ata_read_sector(uint8_t drive, uint32_t lba, uint8_t sectorcount, void* buffer);

#endif
