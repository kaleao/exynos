#!/bin/sh

dd if=set.bin.revb of=/dev/sdb skip=1
dd if=Image_noramfs of=/dev/sda seek=20480
dd if=./kmax-revb-ufs.dtb of=/dev/sda seek=18961
dd if=/dev/zero of=/dev/sda bs=512 count=1
echo -e "n\np\n2\n10\n+1G\nw\n" | fdisk /dev/sda
echo -e "n\np\n3\n26\n+20G\nw\n" | fdisk /dev/sda
mkfs.ext4 /dev/sda2 -F
mkfs.ext4 /dev/sda3 -F
mount /dev/sda2 /mnt/
gunzip rootfs.tar.gz
tar xf rootfs.tar -C /mnt/
cp -raf /root/drivers /mnt/root/drivers

