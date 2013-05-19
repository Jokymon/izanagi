allflags = "-include config.h -include include/generated/autoconf.h -Iinclude"
asflags = "%s -I. -D__ASSEMBLY__ -c" % allflags
cflags = "%s -fvisibility=default -fno-builtin -D__KERNEL__ " % allflags
ldflags = "-nostdlib"

gcc_path = Dir("E:/vm-shared/i386-elf/bin")
asm = gcc_path.File("i386-elf-gcc.exe")
gcc = gcc_path.File("i386-elf-gcc.exe")
cpp = gcc_path.File("i386-elf-cpp.exe")
ld  = gcc_path.File("i386-elf-ld.exe")
objcopy = gcc_path.File("i386-elf-objcopy.exe")
objdump = gcc_path.File("i386-elf-objdump.exe")

env = Environment(
    AS = asm,
    ASFLAGS = asflags,

    CC = gcc,
    CCFLAGS = cflags,
    
    LINK = ld,
    LINKFLAGS = ldflags)

srcs = []

SConscript(dirs=['stage2', 'lib'], exports='env srcs')

srcs.append("boot_hdisk.S")
srcs.append("boot_main.S")

env.Command('barebox.lds', 'barebox.lds.S', "%s -P $SOURCE -I. -DTEXT_BASE=0x00007c00 -o $TARGET" % cpp)

izanagi = env.Program('izanagi', srcs, 
                       LINKFLAGS="-T barebox.lds")

env.Command('izanagi.img', izanagi, "%s -I elf32-i386 -O binary $SOURCE $TARGET" % objcopy)
env.Command('izanagi.lst', izanagi, "%s -S -mi386 -Maddr16,data16,intel $SOURCE > $TARGET" % objdump)
