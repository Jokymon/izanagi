import os

if os.name=="nt":
    tools_parts = ("i386-elf-", ".exe")
else:
    tools_parts = ("", "")

allflags = "-include config.h -include include/generated/autoconf.h -Iinclude"
asflags = "%s -I. -D__ASSEMBLY__ -c" % allflags
cflags = "%s -fvisibility=default -fno-builtin -fno-stack-protector -fno-asynchronous-unwind-tables -D__KERNEL__ " % allflags
ldflags = "-nostdlib"

asm = "%sgcc%s" % tools_parts
gcc = "%sgcc%s" % tools_parts
cpp = "%scpp%s" % tools_parts
ld  = "%sld%s" % tools_parts
objcopy = "%sobjcopy%s" % tools_parts
objdump = "%sobjdump%s" % tools_parts

env = Environment(
    AS = asm,
    ASFLAGS = asflags,

    CC = gcc,
    CCFLAGS = cflags,
    
    LINK = ld,
    LINKFLAGS = ldflags,

    tools = ['mingw'],

    ENV = { 'PATH' : os.environ['PATH'] }
)

srcs = []
static_libs = []

SConscript(dirs=['stage2', 'lib', 'drivers', 'external_libs'],
           exports='env static_libs')

srcs.append("boot_hdisk.S")
srcs.append("boot_main.S")

env.Command('barebox.lds',
            'barebox.lds.S',
            "%s -P $SOURCE -I. -DTEXT_BASE=0x00007c00 -o $TARGET" % cpp)

izanagi = env.Program('izanagi', srcs,
                       LIBS=static_libs,
                       LINKFLAGS="-T barebox.lds")

env.Command('izanagi.img',
            izanagi,
            "%s -I elf32-i386 -O binary $SOURCE $TARGET" % objcopy)
env.Command('izanagi.lst',
            izanagi,
            "%s -S -mi386 -Maddr16,data16,intel $SOURCE > $TARGET" % objdump)
