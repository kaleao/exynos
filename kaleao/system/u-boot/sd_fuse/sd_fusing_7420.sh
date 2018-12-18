#
# Copyright (C) 2011 Samsung Electronics Co., Ltd.
#              http://www.samsung.com/
#
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License version 2 as
# published by the Free Software Foundation.
#
####################################

if [ -z $1 ]
then
    echo "usage: ./sd_fusing_ccioff.sh <SD Reader's device file>"
    exit 0
fi

if [ -b $1 ]
then
    echo "$1 reader is identified."
else
    echo "$1 is NOT identified."
    exit 0
fi

####################################
# fusing images

signed_bl1_position=1
el3_position=17
bl2_position=401
uboot_position=441
tzsw_position=1497
kernel_position=2553
ramdisk_position=43513
dtb_position=96761
ldfw_position=98809
#./codesigner_v23 -v2.3 ../spl/smdk5410-spl.bin bl2.signed.bin Exynos5410_PF4_V23.prv -STAGE2
#<u-boot fusing>
#echo "u-boot fusing"
#dd iflag=dsync oflag=dsync if=./img/u-boot-p632x.bin of=$1 seek=$signed_bl1_position

#<BL1 fusing>
#echo "BL1 fusing"
#dd iflag=dsync oflag=dsync if=./image/bl1 of=$1 seek=$signed_bl1_position

#<EL3 fusing>
#echo "el3 fusing"
#dd iflag=dsync oflag=dsync if=./image/el3 of=$1 seek=$el3_position

#<BL2 fusing>
#echo "BL2 fusing"
#dd iflag=dsync oflag=dsync if=./image/bl2 of=$1 seek=$bl2_position

#<u-boot fusing>
echo "u-boot fusing"
dd iflag=dsync oflag=dsync if=../set.bin of=$1 seek=$signed_bl1_position

#<TrustZone S/W fusing>
#echo "TrustZone S/W fusing"
#dd iflag=dsync oflag=dsync if=./image/tzsw of=$1 seek=$tzsw_position

#<Kernel S/W fusing>
#echo "Kernel S/W fusing"
#cp ../../kernel_3.4.39_CM1/arch/arm/boot/zImage ./zImage
#dd iflag=dsync oflag=dsync if=./zImage of=$1 seek=$kernel_position

#<Ramdisk S/W fusing>
#echo "Ramdisk S/W fusing"
#cp ../../android4.3_JLB_CM1/out/target/product/smdk3472/ramdisk.img ./ramdisk.img
#dd iflag=dsync oflag=dsync if=./ramdisk.img of=$1 seek=$ramdisk_position

####################################
#<Message Display>
echo "U-boot image is fused successfully."
echo "Eject SD card and insert it again."
