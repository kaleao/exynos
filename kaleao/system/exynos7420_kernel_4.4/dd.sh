#!/bin/sh

sudo dd of=/dev/sdd if=arch/arm64/boot/dts/exynos/exynos7-howchip.dtb seek=151552

sudo dd of=/dev/sdd if=arch/arm64/boot/Image seek=4096

#sudo umount /media/cperakis/4036-50A0

#sudo umount /media/cperakis/ad2d37e7-8484-4905-982d-d97c2510838d

sync
