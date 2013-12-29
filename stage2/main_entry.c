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
#include <drivers/filesystem.h>
#include <fat_io_lib/fat_filelib.h>

extern void start_barebox(void);

/**
 * Called plainly from assembler that switches from real to flat mode
 *
 * @note The C environment isn't initialized yet
 */
void uboot_entry(void)
{
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

    fl_listdirectory("/");

    //gotoxy(7, 0);

    while(1) {}
}
