#!/bin/bash

BL_DIR=./bls
OUTPUT_DIR=./output

mkdir -p ${OUTPUT_DIR}

echo "Signing..."

#Change the following once signing is in place...
cp ${BL_DIR}/bl1.signed ${OUTPUT_DIR}/bl1.signed


echo -e "\tel3"
rm -f ${OUTPUT_DIR}/el3.signed
#cp ${BL_DIR}/el3_custom.signed ${OUTPUT_DIR}/

cp ${BL_DIR}/el3.kvm ${OUTPUT_DIR}/el3_custom.signed

dd if=${OUTPUT_DIR}/el3_custom.signed of=${OUTPUT_DIR}/el3_custom.chopped bs=1 count=196336 &> /dev/null

dd if=${OUTPUT_DIR}/el3_custom.chopped of=${OUTPUT_DIR}/el3_customnh.chopped bs=1 skip=16 &> /dev/null

./bin/RSA_v3.0.3 -STAGE2 -IMGMAKE -infile=${OUTPUT_DIR}/el3_customnh.chopped -outfile=${OUTPUT_DIR}/el3.signed -pri=./keys/KaleoKMAXrevB_key.prv -rb_count=0 -dynamic_length=yes &> /dev/null

echo -e "\tbl2"

rm -f ${OUTPUT_DIR}/bl2.signed
cp ${BL_DIR}/bl2_custom.signed ${OUTPUT_DIR}/

dd if=${OUTPUT_DIR}/bl2_custom.signed of=${OUTPUT_DIR}/bl2_custom.chopped bs=1 count=48128 &> /dev/null

./bin/RSA_v3.0.3 -STAGE2 -IMGMAKE -infile=${OUTPUT_DIR}/bl2_custom.chopped -outfile=${OUTPUT_DIR}/bl2.signed -pri=./keys/KaleoKMAXrevB_key.prv -rb_count=0 -dynamic_length=0 &> /dev/null






echo -e "\tu-boot"

rm -f ${OUTPUT_DIR}/u-boot.bin

rm -f ${OUTPUT_DIR}/u-boot.bin.signed

cp ../../u-boot.bin ${OUTPUT_DIR}

dd if=${OUTPUT_DIR}/u-boot.bin of=${OUTPUT_DIR}/u-boot.bin.chopped bs=1 count=540400 &> /dev/null

rm -f ${OUTPUT_DIR}/u-boot.bin
mv -f ${OUTPUT_DIR}/u-boot.bin.chopped ${OUTPUT_DIR}/u-boot.bin

./bin/RSA_v3.0.3 -STAGE2 -IMGMAKE -infile=${OUTPUT_DIR}/u-boot.bin -outfile=${OUTPUT_DIR}/u-boot.bin.signed -pri=./keys/KaleoKMAXrevB_key.prv -rb_count=0 -dynamic_length=0 &> /dev/null

