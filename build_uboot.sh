#!/bin/bash

MYDIR=$PWD
echo $MYDIR
cd ./kaleao/system/u-boot
./build_uboot_kaleao.sh
if [ "$?" == 1 ]; then
	cd $MYDIR
	exit 1
fi
cd $MYDIR

exit 0
