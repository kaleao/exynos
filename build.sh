#!/bin/bash

SDIR=`pwd`
RED='\033[0;31m'
GREEN='\033[0;32m'
BLUE='\033[0;34m'
CYAN='\033[0;36m'
REDLIGHT='\033[1;31m'
GREENLIGHT='\033[1;32m'
BLUELIGHT='\033[1;34m'
CYANLIGHT='\033[1;36m'
NC='\033[0m' # No Color
bold='\033[1m'
normal='\033[0m'
ROOTDIR=`readlink -f .`
TOOLCHAIN="${ROOTDIR}/output/host/usr/bin/aarch64-linux-"

function prinfo {
        echo -e "${CYAN}"$1"${NC}"
}

function check {
	if [ $2 == 0 ] ; then
		echo -e "${GREEN}${bold}Task \"$1\" completed successfully.${normal}${NC}"
	else
		echo -e "${RED}${bold}Task \"$1\" failed.${normal}${NC}"
		cd $SDIR
		exit 1
	fi
}

# Enable recovery build if second argument is "recovery"
# (will be disabled if it is anything else or is omitted)
[ ! $2 ] && RECOVERY="no"
[ "$2" == "recovery" ] && RECOVERY="yes" 

# Set build target and check for validity
BUILD_TARGET=$1
if [ "${BUILD_TARGET}" != "server" ] || [ ! ${RECOVERY} ]; then
    echo -e "Usage:\n\t./build.sh server [recovery]"
    exit 1
fi

# Print settings in case anyone looks at Jenkins logs
echo -e "\nBuilding target \"${CYANLIGHT}${BUILD_TARGET}${NC}\" with recovery"\
	"\b mode set to \"${CYANLIGHT}${RECOVERY}${NC}\","\
	"\b terminal is \"${CYANLIGHT}${TERM}${NC}\".\n"

# FIXME Creating bootloaders have nothing to do with recovery. 
# Must be put in a BR recipe and/or a different build target.
if [ "${RECOVERY}" == "yes" ]; then
	prinfo ">> Building U-Boot"
	./build_uboot.sh
	check "u-boot" $?
fi

## rootfs not to be included in UFS rootfs
rm -f kaleao/system/rootfs_overlay/ufs/rootfs.* output/target/ufs/rootfs.*

if [ "${BUILD_TARGET}" == "server" ]; then
	prinfo ">> Building UFS rootfs"
	cp -f system/device_table_ufs.txt system/device_table.txt
	make kmax_noramfs_defconfig
	make -s
	check "UFS rootfs build" $?

	prinfo ">> Packing UFS rootfs"
	[ -f "output/images/rootfs.tar.gz" ]
	check "UFS rootfs packing" $?

	cp -f output/images/rootfs.tar.gz ./kaleao/system/rootfs_overlay/ufs/

	if [ "${RECOVERY}" == "yes" ]; then
		prinfo ">> Building SD emulation rootfs"
		cp -f ./kaleao/system/misc/S95writeufs ./kaleao/system/rootfs_overlay/etc/init.d/
		cp -f system/device_table_sd.txt system/device_table.txt
		make -s
		check "SD emu rootfs" $?

		prinfo ">> Building SD emu kernel"
		## build SD emulation kernel with embedded ramdisk
		./build_kernel.sh kmax_ramfs_defconfig
		check "SD emu kernel" $?

		prinfo ">> Generating SD emu images"
		cd kaleao/output/
		./make_images.sh
		check "SD emu images" $?
		rm -f {kaleao/system/rootfs_overlay,output/target}/etc/init.d/S95writeufs
	fi
fi

prinfo ">> Building kernel and drivers"
if [ "${BUILD_TARGET}" == "server" ]; then
	./build_kernel.sh kmax_defconfig
	check "kernel" $?
	./build_headers.sh
	check "kernel headers" $?

	for f in `pwd`/kaleao/drivers/*; do
		cd $f
		[ ! -f Makefile ] && continue
		make clean
		make CROSS_COMPILE=${TOOLCHAIN}
		check "kmax kernel module: `basename $f`" $?
	done
	cd ../../..
	
fi

prinfo ">> Copying files to overlay"
## TODO check for failure
## TODO are bootloaders needed for appliance target?!
if [ "${RECOVERY}" == "yes" ]; then
	cp -f kaleao/system/u-boot/kaleao/output/revb-unsec.set.bin ./kaleao/system/rootfs_overlay/ufs/
	cp -f kaleao/system/u-boot/kaleao/output/revb.set.bin ./kaleao/system/rootfs_overlay/ufs/
fi

if [ "${BUILD_TARGET}" == "server" ]; then
	# The kernel
	cp -f ./kaleao/system/exynos7420_kernel_4.4/arch/arm64/boot/Image \
		./kaleao/system/rootfs_overlay/ufs/Image_noramfs
	cp -f ./kaleao/system/exynos7420_kernel_4.4/arch/arm64/boot/dts/exynos/kmax-revb-ubuntu.dtb \
		./kaleao/system/rootfs_overlay/ufs/

	## The drivers
	cp -f ./kaleao/drivers/knet/*.ko ./kaleao/system/rootfs_overlay/root/drivers/
	cp -f ./kaleao/drivers/pmu_status/*.ko ./kaleao/system/rootfs_overlay/root/drivers/

	## Utilities
	mkdir -p ./kaleao/system/rootfs_overlay/root/util
	cp -rf ./kaleao/system/misc/util/* ./kaleao/system/rootfs_overlay/root/util
	cp -rf ./kaleao/system/misc/initscripts/netexy ./kaleao/system/rootfs_overlay/etc/init.d/S41kmaxnet
fi

if [ "${BUILD_TARGET}" == "server" ]; then
	prinfo ">> Generating Ubuntu package"
	[ -f "./kaleao/system/rootfs_overlay/ufs/Image_noramfs" ] && \
		[ -f "./kaleao/system/rootfs_overlay/ufs/kmax-revb-ubuntu.dtb" ] && \
		[ -f "./kaleao/system/misc/initscripts/netexy" ]
	check "gather files for ubuntu package" $?

	cp -f ./kaleao/system/rootfs_overlay/ufs/Image_noramfs ./ubuntu/Image
	cp -f ./kaleao/system/rootfs_overlay/ufs/kmax-revb-ubuntu.dtb ./ubuntu/
	cp -f ./kaleao/system/rootfs_overlay/root/drivers/*.ko ./ubuntu/
	mkdir -p ./ubuntu/util
	cp -f ./kaleao/system/rootfs_overlay/root/util/* ./ubuntu/util
	cp -f ./kaleao/system/misc/initscripts/netexy ./ubuntu
	mkdir -p ./ubuntu/bench
	cp -raf ./kaleao/bench/* ./ubuntu/bench/
	rm -f ./ubuntu.tar.gz
	tar -zcf ./ubuntu.tar.gz ./ubuntu/
	[ "$?" == 0 ] && [ -f "./ubuntu.tar.gz" ]
	check "ubuntu package generation" $?
fi

prinfo ">> Generating deb packages"

# save the build target within the deb file, to be used by both generation script and post-inst
sed -i "s/BUILD_TARGET=\".*\"/BUILD_TARGET=\"${1}\"/" kaleao/updates/packages/recovery/kmax-exynos/*
sed -i "s/RECOVERY=\".*\"/RECOVERY=\"${RECOVERY}\"/" kaleao/updates/packages/recovery/kmax-exynos/*

# FIXME we should avoid generation of bootloader-related debs if bootloader was not built.
./build_debs.sh
check "deb packages" $?


rm -rf output/images/* kaleao/output/images/tmp-img/*
prinfo "Finished successfully."
