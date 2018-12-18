#!/bin/bash

OUTPUT_DIR=./output
BL_DIR=./bls



rm -f ${OUTPUT_DIR}/* &> /dev/null

mkdir -p ../output

rm -f ../output/* &> /dev/null

./sign_bls.sh

BL1=${OUTPUT_DIR}/bl1.signed
BL1_UNS=${BL_DIR}/bl1.unsigned

EL3=${OUTPUT_DIR}/el3.signed
#EL3=el3_chopped.final.signed2
#EL3=lala4
#BL2=bl2_final.signed
#BL2=bl2_padded


BL2=${OUTPUT_DIR}/bl2.signed
#BL2=${OUTPUT_DIR}/../../../spl/u-boot-spl.bin

UBOOT=${OUTPUT_DIR}/u-boot.bin.signed

BL1_POS=1
EL3_POS=$(($BL1_POS+8*2))
BL2_POS=$(($EL3_POS+192*2))
UBOOT_POS=$(($BL2_POS+48*2))

echo "Creating Empty Image"

dd if=/dev/zero of=${OUTPUT_DIR}/set.bin bs=512 count=$(($UBOOT_POS+528*2+5)) &> /dev/null

echo "Creating Signed Image"
echo "Writing:"
echo -e "\tbl1"
dd if=$BL1 of=${OUTPUT_DIR}/set.bin bs=512 seek=$BL1_POS conv=notrunc 
#&> dev/null
echo -e "\tel3"
dd if=$EL3 of=${OUTPUT_DIR}/set.bin bs=512 seek=$EL3_POS conv=notrunc
#&> /dev/null
echo -e "\tbl2"
dd if=$BL2 of=${OUTPUT_DIR}/set.bin bs=512 seek=$BL2_POS conv=notrunc
#&> /dev/null
echo -e "\tu-boot"
dd if=$UBOOT of=${OUTPUT_DIR}/set.bin bs=512 seek=$UBOOT_POS conv=notrunc
#&> /dev/null

cp ${OUTPUT_DIR}/set.bin ../output/revb.set.bin

echo -e "Signed Image generation completed\n"

echo "Creating Non-signed Image"
echo "Writing:"
echo -e "\tbl1"
dd if=$BL1_UNS of=${OUTPUT_DIR}/set.bin bs=512 seek=$BL1_POS conv=notrunc &> /dev/null

cp ${OUTPUT_DIR}/set.bin ../output/revb-unsec.set.bin


echo -e "Unsigned Image generation completed\n"

echo "Creating Bare-metal .h Images"

./convert ../output/revb.set.bin

mv ./myfatdisk.h ../output/revb.set.bin.h

./convert ../output/revb-unsec.set.bin

mv ./myfatdisk.h ../output/revb-unsec.set.bin.h
