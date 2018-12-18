#!/bin/sh

func_help() {
	echo "Usage: ./build_uboot_7420.sh <avl7420 | espresso7420> [sdcard]"
	echo "If you want to make fuse sdcard, pls add 'sdcard' parameter to compile"
	echo "How to make fusing card , pls refer to sdfuse/HowtoMakeFusingCard.TXT"
	exit 1
}

if [ $1 ]
then
	if [ $1 != "avl7420" -a $1 != "espresso7420" ]
	then
		func_help
	fi

else
	func_help
fi

if [ $2 ]
then
	if [ $2 != "sdcard" ]
	then
		func_help
	fi
fi

BOARD=""
TOOL=""
CONFIG=""

if [ $1 = "avl7420" ]
then
	BOARD="avl7420"
	TOOL="mkavl7420spl47kb"
	CONFIG="avl7420_aarch64_evt1"
elif [ $1 = "espresso7420" ]
then
	BOARD="espresso7420"
	TOOL="mkespresso7420spl47kb"
	CONFIG="espresso7420_aarch64_evt1"
else
	echo % wrong board name
	echo % usage : ./build.sh [board name]
	echo % board list
	echo avl7420
	echo espresso7420
	exit -1
fi


rm tools/*kb
rm -rf output
mkdir -p output

grep -c "#define SD_FIRM_IN	0" include/configs/$1_aarch64_evt1.h
#when not found it, $? return 1; when found it, $? return 0
if [ $? -eq 1  -a $# -lt 2 ]
then
	sed -i 's:#define SD_FIRM_IN	1:#define SD_FIRM_IN	0:g' include/configs/$1_aarch64_evt1.h
	sed -i 's:#define SD_FIRM_IN	1:#define SD_FIRM_IN	0:g' include/configs/$1_aarch64_evt1_SG.h
fi

grep -c "#define SD_FIRM_IN	0" include/configs/$1_aarch64_evt1.h
if [ $? -eq 0 ]
then
	if [ $2 ]
	then
		if [ $2 = "sdcard" ]
		then
			sed -i 's:#define SD_FIRM_IN	0:#define SD_FIRM_IN	1:g' include/configs/$1_aarch64_evt1.h
			sed -i 's:#define SD_FIRM_IN	1:#define SD_FIRM_IN	0:g' include/configs/$1_aarch64_evt1_SG.h

		fi
	fi
fi

if [ $1 = "avl7420" ]
then
make distclean ${CONFIG}_SG_config
make -j4
mv spl/u-boot-spl output/u-boot-spl.sg
mv spl/u-boot-spl.bin output/u-boot-spl.bin.sg
mv spl/$BOARD-spl.bin output/$BOARD-spl.bin.sg

make distclean ${CONFIG}_config
make -j4
mv spl/u-boot-spl output/u-boot-spl.org
mv spl/u-boot-spl.bin output/u-boot-spl.bin.org
mv spl/$BOARD-spl.bin output/$BOARD-spl.bin.org

if [ ! -f u-boot.bin ]
then
	echo "**************************"
	echo "COMPILE error ; Please check code  !!!"
	echo "**************************"
	exit -1
fi
./add_padding u-boot.bin 540672
cat output/$BOARD-spl.bin.org output/$BOARD-spl.bin.sg > output/$BOARD-spl.bin.tmp
tools/$TOOL output/$BOARD-spl.bin.tmp output/$BOARD-spl.bin
#cp output/$BOARD-spl.bin ./
#./add_padding $BOARD-spl.bin 49152
#cp output/$BOARD-spl.bin ./sd_fuse/image_secure/bl2
#cp ./u-boot.bin ./sd_fuse/image_secure/bootloader
if [ -f ./sd_fuse/image_secure/signed_bl1 -a -f ./sd_fuse/image_secure/signed_el3 -a -f ./sd_fuse/image_secure/signed_bl2 -a -f ./sd_fuse/image_secure/signed_tzsw -a -f ./sd_fuse/image_secure/signed_bootloader ]
then
	cat ./sd_fuse/image_secure/signed_bl1 ./sd_fuse/image_secure/signed_el3 ./sd_fuse/image_secure/signed_bl2 ./sd_fuse/image_secure/signed_bootloader ./sd_fuse/image_secure/signed_tzsw > set.bin
	rm u-boot.bin
	cp set.bin ./sd_fuse/image_secure/
fi

elif [ $1 = "espresso7420" ]
then
make distclean ${CONFIG}_config
make -j4
if [ ! -f u-boot.bin ]
then
	echo "**************************"
	echo "COMPILE error ; Please check code  !!!"
	echo "**************************"
	exit -1
fi
./add_padding u-boot.bin 540672
cp spl/$BOARD-spl.bin ./
./add_padding $BOARD-spl.bin 20480
cp ./$BOARD-spl.bin ./sd_fuse/image/bl2
cp ./u-boot.bin ./sd_fuse/image/bootloader
if [ -f ./sd_fuse/image/bl1 -a -f ./sd_fuse/image/el3 -a -f ./sd_fuse/image/tzsw ]
then
	cat ./sd_fuse/image/bl1 ./sd_fuse/image/el3_smdk ./$BOARD-spl.bin u-boot.bin ./sd_fuse/image/tzsw_smdk > set.bin
	rm u-boot.bin
	cp set.bin ./sd_fuse/image/
fi
fi

 
if [ $2 ]
then
	echo "**************************"
	echo "COMPILE successful ; This uboot is for sdcard firm in , donn't firm in EMMC !!!"
	echo "**************************"
fi

