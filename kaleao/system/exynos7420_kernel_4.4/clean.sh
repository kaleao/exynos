#!/bin/sh

sudo make ARCH=arm64 CROSS_COMPILE=aarch64-linux-gnu- -j8 clean
//scp arch/arm64/boot/dts/exynos/exynos7-espresso.dtb  cperakis@139.91.204.101:~/sdfuse/
//scp arch/arm64/boot/Image  cperakis@139.91.204.101:~/sdfuse/
