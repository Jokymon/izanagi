e:\vm-shared\i386-elf\bin\i386-elf-gcc.exe -c -o boot_main.o boot_main.S
e:\vm-shared\i386-elf\bin\i386-elf-gcc.exe -c -o boot_hdisk.o boot_hdisk.S

e:\vm-shared\i386-elf\bin\i386-elf-cpp.exe -P -I. barebox.lds.S -o barebox.lds -DTEXT_BASE=0x7c00
e:\vm-shared\i386-elf\bin\i386-elf-ld.exe -nostdlib -T barebox.lds -e _start -o mbr.elf boot_main.o boot_hdisk.o

e:\vm-shared\i386-elf\bin\i386-elf-objcopy.exe -O binary mbr.elf mbr.bin
e:\vm-shared\i386-elf\bin\i386-elf-objdump.exe -d mbr.elf
