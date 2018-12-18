#!/bin/bash
export EXYNOS_KERNEL310_PATH=/home/jveleg/kmax/exynos/linux-3.10.0-exynos

clear
make clean
make ARCH=arm64 CROSS_COMPILE=/home/jveleg/kmax/tools/gcc-linaro-4.9-2014.11-x86_64_aarch64-linux-gnu/bin/aarch64-linux-gnu-
