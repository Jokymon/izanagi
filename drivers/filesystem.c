#include <drivers/filesystem.h>
#include <drivers/ata.h>
#include <fat_io_lib/fat_filelib.h>

int media_read(unsigned long sector, unsigned char *buffer, unsigned long sector_count)
{
    ata_read_sector(0, sector+150, sector_count, buffer);
    return 1;
}

int media_write(unsigned long sector, unsigned char *buffer, unsigned long sector_count)
{
    // TODO: Implement writing
    printf("You're not supposed to write anything at this point\n");
    return 0;
}

void init_filesystem(void* partition_table, int partition)
{
    // TODO: take partition_table address and partition into account to fetch
    // actual values
    fl_init();
    if (fl_attach_media(media_read, media_write) != FAT_INIT_OK)
    {
        printf("ERROR: Media attach failed\n");
        while (1) {}
    }
}
