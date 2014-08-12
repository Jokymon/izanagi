#include <io.h>
#include "drivers/ata.h"

#define HDC1_DATA 0x1f0
#define HDC1_ERROR 0x1f1
#define HDC1_WPC4 0x1f1
#define HDC1_SECTOR_COUNT 0x1f2
#define HDC1_SECTOR_NUMBER 0x1f3
#define HDC1_CYL_LOW 0x1f4
#define HDC1_CYL_HIGH 0x1f5
#define HDC1_DRIVEHEAD 0x1f6
#define HDC1_STATUS 0x1f7
#define HDC1_COMMAND 0x1f7

// 0x170  -- 2nd HD CTRL

#define ATA_COMMAND_READ 0x20
#define ATA_COMMAND_READ_NO_RETRY 0x21
#define ATA_COMMAND_WRITE 0x30

#define ATA_STATUS_DRIVE_BUSY 0x80
#define ATA_STATUS_DRIVE_READY 0x40
#define ATA_STATUS_WRITE_FAULT 0x20
#define ATA_STATUS_SEEK_COMPLETE 0x10

bool ata_drive_ready(uint16_t status_register)
{
    return ((inb(status_register) & ATA_STATUS_DRIVE_READY) == ATA_STATUS_DRIVE_READY);
}

bool ata_drive_busy(uint16_t status_register)
{
    return ((inb(status_register) & ATA_STATUS_DRIVE_BUSY) == ATA_STATUS_DRIVE_BUSY);
}

void ata_read_sector(uint8_t drive, uint32_t lba, uint8_t sectorcount, void* buffer)
{
    uint16_t *_buffer = (uint16_t*)buffer;
    outb(HDC1_WPC4, 0x00);
    outb(HDC1_SECTOR_COUNT, sectorcount);
    outb(HDC1_SECTOR_NUMBER, (uint8_t)lba);
    outb(HDC1_CYL_LOW, (uint8_t)(lba>>8));
    outb(HDC1_CYL_HIGH, (uint8_t)(lba>>16));
    // TODO: drive can only be 0 or 1, check somewhere
    outb(HDC1_DRIVEHEAD, 0xe0 | (drive << 4) | ((lba >> 24) & 0x0f));
    outb(HDC1_COMMAND, ATA_COMMAND_READ);

    while (ata_drive_busy(HDC1_STATUS)) {}

    uint32_t i;
    for (i=0; i < (sectorcount*256); i++)
    {
        _buffer[i] = inw(HDC1_DATA);
    }
}
