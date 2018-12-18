#!/bin/sh

if [ ! -e /dev/sda ] || [ ! -e /dev/sdb ]; then
	echo "UFS not present!"
	exit 1;
fi

dd if=revb-unsec.set.bin of=/dev/sdb skip=1
dd if=Image_noramfs of=/dev/sda seek=20480
dd if=./kmax-revb-ufs.dtb of=/dev/sda seek=18961
dd if=./kmax-revb-ubuntu.dtb of=/dev/sda seek=225280
dd if=Image_noramfs of=/dev/sda seek=245760
dd if=/dev/zero of=/dev/sda bs=512 count=1
echo -e "n\np\n2\n17\n80\nw\n" | fdisk /dev/sda
echo -e "n\np\n3\n81\n1800\nw\n" | fdisk /dev/sda
echo -e "n\np\n4\n1801\n\n\nw\n" | fdisk /dev/sda
mkfs.ext4 /dev/sda2 -F
mkfs.ext4 /dev/sda3 -F
mount /dev/sda2 /mnt/
gunzip rootfs.tar.gz
tar pxf rootfs.tar -C /mnt/ --warning=no-timestamp
cp -raf /root/drivers/* /mnt/root/drivers/
chmod +x /mnt/etc/init.d/*
chmod 755 /mnt/root/util/*
rm -rf /mnt/etc/ssh/*host*
sync
umount /mnt
sync

mount /dev/sda2 /mnt/

if [ ! -d /mnt/root/ ]; then
	echo "UFS not written!"
	exit 1;
fi

umount /mnt

exit 0;

