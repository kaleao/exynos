#!/bin/bash

echo "dd kernel to : $1"
#dd if=arch/arm64/boot/Image of=$1 seek=4096

dd if=arch/arm64/boot/Image of=$1 seek=2553

echo "dd dtb to : $1"
#dd iflag=dsync oflag=dsync if=arch/arm64/boot/dts/exynos/exynos7-howchip.dtb of=$1 seek=151552


dd iflag=dsync oflag=dsync if=arch/arm64/boot/dts/exynos/exynos7-howchip.dtb of=$1 seek=96761
