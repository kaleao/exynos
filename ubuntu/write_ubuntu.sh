#!/bin/sh

dd if=./kmax-revb-ubuntu.dtb of=/dev/sda seek=225280
dd if=./Image of=/dev/sda seek=245760
mkfs.ext4 /dev/sda3 -F
sync

./write_ubuntu_rootfs.sh
