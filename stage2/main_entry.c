/*
 * Copyright (C) 2009 Juergen Beisert, Pengutronix
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 *
 */

/**
 * @file
 * @brief Start of the 32 bit flat mode
 */

#include <string.h>
#include <stdio.h>
#include <asm/sections.h>
#include <disks.h>
#include <drivers/filesystem.h>
#include <fat_io_lib/fat_filelib.h>

#include <module.h>

extern void start_barebox(void);

/**
 * Called plainly from assembler that switches from real to flat mode
 *
 * @note The C environment isn't initialized yet
 */
void uboot_entry(void)
{
    int result;

	/* clear the BSS first */
	memset(__bss_start, 0x00, __bss_stop - __bss_start);
    // TODO: jump to Izanagi code
	//start_barebox();

    clrscr();
    printf("Izanagi Version %u.%u\n", 1, 0);
    printf("==================================\n");
    printf("\n");
    printf("Video base address: 0x%x", 0xb0000);

    init_filesystem(0x0, 0);

#if 0
    struct partition_entry* pt = (struct partition_entry*)(TEXT_BASE + 0x1BE);

    printf("Partition entry 0:\n");
    printf("Bootable flag: 0x%x\n", pt->boot_indicator);
    printf("Filesystem type: 0x%x\n", pt->type);
    printf("Partition start: %u\n", pt->partition_start);
    printf("Partition size: %u\n", pt->partition_size);
#endif

    result = load_kernel_image("/hirvi.elf");
    if (0 == result)
    {
        printf("Loading kernel image failed: %d\n", result);
    }

    while(1) {}
}
