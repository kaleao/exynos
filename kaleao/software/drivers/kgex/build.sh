#!/bin/bash

CWD=`pwd`

# Find Network FPGA Linux kernel absolute path
cd ../../exynos7420_kernel_4.4
KERNEL_PATH=`pwd`
export EXYNOS_KERNEL_PATH=$KERNEL_PATH

cd $CWD

# Find cross compiler absolute path
cd ../../gcc-linaro-aarch64-linux-gnu-4.8-2014.04_linux
CROSS_PATH=`pwd`
#echo $CROSS_PATH

cd $CWD

clear
make clean
make ARCH=arm64 CROSS_COMPILE=$CROSS_PATH/bin/aarch64-linux-gnu-
