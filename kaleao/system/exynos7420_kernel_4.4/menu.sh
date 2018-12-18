#!/bin/sh

sudo make ARCH=arm64 CROSS_COMPILE=../gcc-linaro-aarch64-linux-gnu-4.8-2014.04_linux/bin/aarch64-linux-gnu- -j8 menuconfig
