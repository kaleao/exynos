#!/bin/sh

dd if=revb.set.bin of=/dev/sdb skip=1
dd if=Image_noramfs of=/dev/sda seek=20480
dd if=./kmax-revb-ufs.dtb of=/dev/sda seek=18961
dd if=/dev/zero of=/dev/sda bs=512 count=1
echo -e "n\np\n2\n2000\n+1G\nw\n" | fdisk /dev/sda
echo -e "n\np\n3\n2120\n+20G\nw\n" | fdisk /dev/sda
mkfs.ext4 /dev/sda2 -F
mkfs.ext4 /dev/sda3 -F
mount /dev/sda2 /mnt/
gunzip rootfs.tar.gz
tar pxf rootfs.tar -C /mnt/ --warning=no-timestamp
cp -raf /root/drivers /mnt/root/drivers

chmod +x /mnt/etc/init.d/*
sync
umount /mnt
sync
