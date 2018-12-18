#!/bin/bash


rm -rf ./images
rm -rf ./tmp-img

rm -f ../../output/images/kmax.dtb
cp -f ../../kaleao/system/exynos7420_kernel_4.4/arch/arm64/boot/dts/exynos/kmax-revb-sdcard.dtb ../../output/images/kmax.dtb

rm -f ../../output/images/Image
cp -f ../../kaleao/system/exynos7420_kernel_4.4/arch/arm64/boot/Image ../../output/images/

rm -f ../../output/images/revb-unsec.set.bin
cp -f ../../kaleao/system/u-boot/kaleao/output/revb-unsec.set.bin ../../output/images/

rm -f ../../output/images/revb.set.bin
cp -f ../../kaleao/system/u-boot/kaleao/output/revb.set.bin ../../output/images/

## Genarate non-secure image
../../output/host/usr/bin/genimage \
	--rootpath ../../output/target \
	--tmppath ./tmp-img \
	--inputpath ../../output/images \
	--outputpath ./images \
	--config ./genimage-revb-unsec.cfg

rm -rf ./tmp-img

## Generate secure image
../../output/host/usr/bin/genimage \
	--rootpath ../../output/target \
	--tmppath ./tmp-img \
	--inputpath ../../output/images \
	--outputpath ./images \
	--config ./genimage-revb.cfg
