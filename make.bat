e:\vm-shared\i386-elf\bin\i386-elf-gcc.exe -Iinclude -D__ASSEMBLY__ -c stage2\*.S
e:\vm-shared\i386-elf\bin\i386-elf-gcc.exe -fno-builtin -include include/generated/autoconf.h -Iinclude -D__KERNEL__ -c stage2\*.c
e:\vm-shared\i386-elf\bin\i386-elf-gcc.exe -fno-builtin -include include/generated/autoconf.h -Iinclude -D__KERNEL__ -c lib\*.c

e:\vm-shared\i386-elf\bin\i386-elf-gcc.exe -c -o boot_main.o boot_main.S
e:\vm-shared\i386-elf\bin\i386-elf-gcc.exe -c -o boot_hdisk.o boot_hdisk.S

e:\vm-shared\i386-elf\bin\i386-elf-cpp.exe -P -I. barebox.lds.S -o barebox.lds -DTEXT_BASE=0x7c00
e:\vm-shared\i386-elf\bin\i386-elf-ld.exe -nostdlib -T barebox.lds -e _start -o mbr.elf *.o
REM boot_main.o boot_hdisk.o

e:\vm-shared\i386-elf\bin\i386-elf-objcopy.exe -O binary mbr.elf mbr.bin
e:\vm-shared\i386-elf\bin\i386-elf-objdump.exe -S -mi386 -Maddr16,data16,intel -d mbr.elf > mbr.lst
