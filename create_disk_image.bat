tools\pyvirtualfs\pyvfs.py create test_10m.img 10080k
tools\pyvirtualfs\pyvfs.py fdisk create image://test_10m.img:hd/partition0 start=150 end=10240 type=6
tools\pyvirtualfs\pyvfs.py mkfs image://test_10m.img:hd/partition0
setupmbr.py -m izanagi.img -d test_10m.img
