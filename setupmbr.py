# Copyright (C) 2009 Juergen Beisert, Pengutronix
#
# This program is free software; you can redistribute it and/or
# modify it under the terms of the GNU General Public License as
# published by the Free Software Foundation; either version 2 of
# the License, or (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 59 Temple Place, Suite 330, Boston,
# MA 02111-1307 USA
#

#
# @file
# @brief Write the barebox binary to the MBR and the following disk sectors
#
# Also patch dedicated locations in the image to make it work at runtime
#
# Current restrictions are:
# - only installs into MBR and the sectors after it
# - tested only with QEMU
# - and maybe some others
#

import os.path

script_path = os.path.dirname(__file__)

def values2string(values):
    import sys
    if sys.version_info[0]==2:
        return "".join(map(chr, values))
    else:
        return bytes(values)

def populate_globals_from_header(header_file):
    f = open(header_file, "r")
    for line in f.readlines():
        line = line.strip()
        if line.startswith("#define"):
            (variable, value) = line[len("#define"):].strip().split(" ")
            globals()[variable] = eval(value)

populate_globals_from_header(os.path.join(script_path, "barebox.lds.h"))

class Field(object):
    def __init__(self, start, size):
        self.start = start
        self.size = size

    def __get__(self, instance, owner):
        start = instance.start_offset + self.start
        values = instance.array[start:start+self.size]
        powers = range(len(values))
        powers = map(lambda x: 256**x, powers)
        summands = map(int.__mul__, values, powers)
        return sum(summands)

    def __set__(self, instance, value):
        if value >= 2**(self.size*8):
            raise ValueError("%u does not fit in this field" % value)
        powers = []
        while value!=0:
            powers.append( int(value % 256) )
            value /= 256
        powers.extend( self.size*[0] )
        start = instance.start_offset + self.start

        instance.array[start:start+self.size] = values2string(powers[0:self.size])

class ClassWithLength(type):
    def __len__(self):
        return self.clslength()

class StructTemplate(object, metaclass=ClassWithLength):
    #__metaclass__ = ClassWithLength

    @classmethod
    def clslength(cls):
        len = 0
        for attr in cls.__dict__.values():
            if isinstance(attr, Field):
                len += attr.size
        return len

    def __len__(self):
        return len(self.__class__)

class DAPS(StructTemplate):
    def __init__(self, array, start_offset):
        self.array = array
        self.start_offset = start_offset

    size = Field(0, 1)
    res1 = Field(1, 1)
    count = Field(2, 1)
    res2 = Field(3, 1)
    offset = Field(4, 2)
    segment = Field(6, 2)
    lba = Field(8, 8)

class Partition(StructTemplate):
    def __init__(self, array, start_offset):
        self.array = array
        self.start_offset = start_offset

    boot_indicator = Field(0, 1)
    chs_begin = Field(1, 3)
    type = Field(4, 1)
    chs_end = Field(5, 3)
    partition_start = Field(8, 4)
    partition_size  = Field(12, 4)

def target2host_32(value):
    return value

def host2target_16(value):
    return value

def host2target_64(value):
    return value

class SetupMbrError:
    pass

def fill_daps(sector, count, offset, segment, lba):
    assert count < 128
    assert offset < 0x10000
    assert segment < 0x10000

    sector.size = len(sector)
    sector.res1 = 0
    sector.count = count
    sector.res2 = 0
    sector.offset = host2target_16(offset)
    sector.segment = host2target_16(segment)
    sector.lba = host2target_64(lba)

    return True

def invalidate_daps(sector):
    sector.size = MARK_DAPS_INVALID
    sector.res1 = 0

def barebox_linear_image(hd_image, daps_table, size):
    offset = LOAD_AREA
    segment = LOAD_SEGMENT
    i = 0
    lba = 2

    size -= 2 * SECTOR_SIZE
    size = (size + SECTOR_SIZE - 1) & ~(SECTOR_SIZE - 1)

    if (size >= (SECTOR_SIZE / len(DAPS) - 1) * 32 * 1024):
        print("Image too large to boot. Max size is %u kiB, image size is %u kiB" %
            ((SECTOR_SIZE / len(DAPS) - 1) * 32, size / 1024)
            )
        return False

    if size > 32 * 1024:
        next_offset = (offset + 32 * 1024 -1) & ~0x7fff
        chunk_size = next_offset - offset
        if chunk_size & (SECTOR_SIZE-1):
            print("Unable to pad from %X to %X in multiple of sectors" % (offset, next_offset))
            return False

        rc = fill_daps(DAPS(hd_image, daps_table+i*len(DAPS)), chunk_size / SECTOR_SIZE, offset, segment, lba)
        if not rc:
            print("Couldn't fill the DAPS")
            return False

        size -= chunk_size
        i += 1
        lba += chunk_size / SECTOR_SIZE
        offset += chunk_size
        if offset >= 0x10000:
            segment += 4096
            offset = 0

        while size:
            if size >= 32 * 1024:
                if i >= (SECTOR_SIZE / len(DAPS)):
                    print("Internal tool error: Too many DAPS entries!")
                    return False
                rc = fill_daps(DAPS(hd_image, daps_table+i*len(DAPS)), 64, offset, segment, lba)
                if not rc:
                    return False
                
                size -= 32 * 1024
                lba += 64
                offset += 32 * 1024
                if offset >= 0x10000:
                    segment += 4096
                    offset = 0
                i += 1
            else:
                if i >= SECTOR_SIZE / len(DAPS):
                    print("Internal tool error: Too many DAPS entries!")
                    return False
                rc = fill_daps(DAPS(hd_image, daps_table+i*len(DAPS)), size / SECTOR_SIZE, offset, segment, lba)
                if not rc:
                    return False
                size = 0
                i += 1
    else:
        rc = fill_daps(DAPS(hd_image, daps_table+i*len(DAPS)), size / SECTOR_SIZE, offset, segment, lba)
        if not rc:
            return False
        i += 1

    if i >= (SECTOR_SIZE / len(DAPS)):
        return True

    # mark the last DAPS invalid
    invalidate_daps( DAPS(hd_image, daps_table + i*len(DAPS)) )

    return True

def check_for_valid_mbr(sector, size):
    if size < SECTOR_SIZE:
        print("MBR too small to be valid")
        return False
    if (sector[OFFSET_OF_SIGNATURE] != 0x55) or \
       (sector[OFFSET_OF_SIGNATURE+1] != 0xAA):
        print("No MBR signature found")
        return False
    return True

def check_for_space(hd_image, size):
    if not check_for_valid_mbr(hd_image, size):
        return False

    partition = Partition(hd_image, OFFSET_OF_PARTITION_TABLE)

    spare_sector_count = target2host_32( partition.partition_start )

    print("Debug: Required free sectors for barebox prior first partition: %u, hd image provides: %u" % (
           (size + SECTOR_SIZE - 1) / SECTOR_SIZE,
           spare_sector_count))

    spare_sector_count *= SECTOR_SIZE
    if spare_sector_count < size:
        print("Not enough space after MBR to store minibox")
        print("Move begin of the first partition beyond sector %u" % ((size + SECTOR_SIZE - 1) / SECTOR_SIZE))
        return False
    return True

def barebox_overlay_mbr(fd_barebox, fd_hd):
    import mmap, os
    sb = os.fstat(fd_barebox.fileno())
    barebox_image = mmap.mmap(fd_barebox.fileno(), 0, access=mmap.ACCESS_READ)

    check_for_valid_mbr(barebox_image, sb.st_size)

    required_size = sb.st_size
    hd_image = mmap.mmap(fd_hd.fileno(), required_size, access=mmap.ACCESS_WRITE)

    check_for_space(hd_image, required_size)

    # embed barebox's boot code into the disk drive image
    hd_image[0:OFFSET_OF_PARTITION_TABLE] = barebox_image[0:OFFSET_OF_PARTITION_TABLE]

	# embed the barebox main image into the disk drive image,
	# but keep the persistant environment storage untouched
	# (if defined), e.g. store the main image behind this special area.
    hd_image_start = SECTOR_SIZE
    barebox_image_start = SECTOR_SIZE
    size = sb.st_size - SECTOR_SIZE
    hd_image[hd_image_start:hd_image_start+size] = barebox_image[barebox_image_start:barebox_image_start+size]

    embed = PATCH_AREA
    indirect = SECTOR_SIZE

    fill_daps(DAPS(hd_image, embed), 1, INDIRECT_AREA, INDIRECT_SEGMENT, 1)

    rc = barebox_linear_image(hd_image, indirect, sb.st_size)
    if not rc:
        return False

    hd_image.close()
    barebox_image.close()

def main(argv):
    from optparse import OptionParser
    parser = OptionParser()
    parser.add_option("-m", dest="barebox_image_filename",
                      help="")
    parser.add_option("-d", dest="hd_image_filename",
                      help="")

    (options, args) = parser.parse_args(argv)

    fd_barebox_image = open(options.barebox_image_filename, "r+b")
    fd_hd_image = open(options.hd_image_filename, "a+b")

    barebox_overlay_mbr(fd_barebox_image, fd_hd_image)

    fd_barebox_image.close()
    fd_hd_image.close()

if __name__=="__main__":
    import sys
    main(sys.argv[1:])
