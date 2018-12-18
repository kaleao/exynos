#!/bin/sh

mkfs.ext4 /dev/sda3 -F
sync
mount /dev/sda3 /mnt/
sync
gunzip ubuntu_rootfs.tar.gz
sync
tar pxf ./ubuntu_rootfs.tar -C /mnt/ --warning=no-timestamp
sync
mkdir -p /mnt/root/drivers
cp -raf ./*.ko /mnt/root/drivers
sync
mkdir -p /mnt/root/util
cp -raf ./util/* /mnt/root/util/
sync
cp -af ./kernel_modules/lib /mnt/
chown  root:root /mnt/lib/
chown -R root:root /mnt/lib/modules
sync
cp -af ./kernel_headers/usr /mnt/
chown  root:root /mnt/usr/
chown -R root:root /mnt/usr/src
find /mnt/usr/src/ -exec touch {} +
sync
cp -raf ./netexy /mnt/etc/init.d/
cp -raf ./irqbalance /mnt/etc/default/
sync
cp -raf ./kmax_set_update_repo /mnt/etc/dhcp/dhclient-enter-hooks.d/
sync

MYDIR=`pwd`

cd /mnt/etc/rc0.d
ln -s ../init.d/netexy K01netexy

cd /mnt/etc/rc2.d
ln -s ../init.d/netexy S01netexy

cd /mnt/etc/rc4.d
ln -s ../init.d/netexy S01netexy

cd /mnt/etc/rc5.d
ln -s ../init.d/netexy S01netexy

cd /mnt/etc/rc6.d
ln -s ../init.d/netexy K01netexy
sync

cd $MYDIR

rm -rf /mnt/etc/ssh/ssh_host*
sync

rm -f /mnt/var/lib/dpkg/lock
rm -f /mnt/var/lib/apt/lists/lock

cp -raf ./journald.conf /mnt/etc/systemd/

mkdir -p /mnt/var/log/journal

sync

umount /mnt
sync


