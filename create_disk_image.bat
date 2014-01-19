tools\pyvirtualfs\pyvfs.py create izanagi_hd.img 18M
tools\pyvirtualfs\pyvfs.py fdisk create image://izanagi_hd.img:hd/partition0 start=150 end=16550 type=6
tools\pyvirtualfs\pyvfs.py mkfs image://izanagi_hd.img:hd/partition0
tools\pyvirtualfs\pyvfs.py copy ..\hirvi\kernel.elf image://izanagi_hd.img:hd/partition0/kernel.elf
setupmbr.py -m izanagi.img -d izanagi_hd.img
