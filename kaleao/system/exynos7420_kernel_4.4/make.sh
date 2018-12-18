#!/bin/sh
make ARCH=arm64 CROSS_COMPILE=$(readlink -f ../)/gcc-linaro-aarch64-linux-gnu-4.8-2014.04_linux/bin/aarch64-linux-gnu- kmax_defconfig
make ARCH=arm64 CROSS_COMPILE=../gcc-linaro-aarch64-linux-gnu-4.8-2014.04_linux/bin/aarch64-linux-gnu- -j8
