#!/bin/bash

RED='\033[0;31m'
GREEN='\033[0;32m'
BLUE='\033[0;34m'
CYAN='\033[0;36m'
REDLIGHT='\033[1;31m'
GREENLIGHT='\033[1;342m'
BLUELIGHT='\033[1;34m'
CYANLIGHT='\033[1;36m'
NC='\033[0m' # No Color
bold=$(tput bold)
normal=$(tput sgr0)

function prerr {
        echo -e "${RED}${bold}ERROR: "$1"${normal}${NC}"
}


function prinfo {
        echo -e "${CYANLIGHT}"$1"${NC}"
}

function prok {
        echo -e "${GREEN}${bold}"$1"${normal}${NC}"
}


#make CROSS_COMPILE=$(readlink -f ../)/gcc-linaro-aarch64-linux-gnu-4.8-2014.04_linux/bin/aarch64-linux-gnu- clean
#make CROSS_COMPILE=$(readlink -f ../)/gcc-linaro-aarch64-linux-gnu-4.8-2014.04_linux/bin/aarch64-linux-gnu- distclean
rm -f ./kaleao/output/revb.set.bin
rm -f ./kaleao/output/revb-unsec.set.bin
rm -f ./u-boot.bin

make CROSS_COMPILE=$(readlink -f ../)/gcc-linaro-aarch64-linux-gnu-4.8-2014.04_linux/bin/aarch64-linux-gnu- kmax_config

#make CROSS_COMPILE=$(readlink -f ../)/gcc-linaro-aarch64-linux-gnu-4.8-2014.04_linux/bin/aarch64-linux-gnu- u-boot.bin


make CROSS_COMPILE=$(readlink -f ../)/gcc-linaro-aarch64-linux-gnu-4.8-2014.04_linux/bin/aarch64-linux-gnu-
#if [ "$?" != 0 ]; then
#	prerr "Failed to compile U-Boot"
 #       exit 1
#fi

if [ ! -f "./u-boot.bin" ]; then
	prerr "Failed to compile U-Boot"
	exit 1
fi

./add_padding u-boot.bin 540672 &> /dev/null

cd kaleao/signing

mkdir -p kaleao/output

./make_boot_image.sh
cd - &> /dev/null

if [ ! -f "./kaleao/output/revb.set.bin" ] || [ ! -f "./kaleao/output/revb-unsec.set.bin" ]; then
	prerr "Failed to create Boot Images"
	exit 1
fi

prinfo "Boot Images created in: $(pwd)/kaleao/output"

exit 0
