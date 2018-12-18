#!/bin/bash

KDIR=kaleao/system/exynos7420_kernel_4.4
TOOLCHAIN=`readlink -f output/host/usr/bin/aarch64-buildroot-linux-gnu-`
MOD_TARGET=`readlink -f ubuntu/kernel_modules/`
make -C $KDIR ARCH=arm64 CROSS_COMPILE=$TOOLCHAIN $1 || exit 1
make -C $KDIR ARCH=arm64 CROSS_COMPILE=$TOOLCHAIN -j8 || exit 1
make -C $KDIR ARCH=arm64 CROSS_COMPILE=$TOOLCHAIN INSTALL_MOD_PATH=$MOD_TARGET modules_install || exit 1
 
cd ubuntu/kernel_modules/lib/modules/*/
rm build source
ln -s /usr/src/$(ls ../) build
ln -s /usr/src/$(ls ../) source
cd -

exit 0
