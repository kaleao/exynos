#!/bin/sh

#MYDIR=$PWD

#bash -c "cd ./kaleao/system/u-boot; ./build_uboot_kaleao.sh; exit"
#cd $MYDIR


cp ./board/kaleao/set.bin.reva ${BINARIES_DIR}/
cp ${BINARIES_DIR}/kmax-reva-sdcard.dtb ${BINARIES_DIR}/kmax.dtb

BOARD_DIR="$(dirname $0)"
GENIMAGE_CFG="${BOARD_DIR}/genimage-reva.cfg"
GENIMAGE_TMP="${BUILD_DIR}/genimage-reva.tmp"


rm -rf "${GENIMAGE_TMP}"

genimage                           \
	--rootpath "${TARGET_DIR}"     \
	--tmppath "${GENIMAGE_TMP}"    \
	--inputpath "${BINARIES_DIR}"  \
	--outputpath "${BINARIES_DIR}" \
	--config "${GENIMAGE_CFG}"

#cp ./board/kaleao/set.bin ${BINARIES_DIR}/
#dd if=${BINARIES_DIR}/set.bin of=${BINARIES_DIR}/sdcard.img seek=1 

#cp ./board/kaleao/set.bin.revb ${BINARIES_DIR}/
echo  "Binaries DIR: ${BINARIES_DIR}"
cp ./kaleao/system/u-boot/kaleao/output/revb.set.bin ${BINARIES_DIR}/set.bin.revb

cp ${BINARIES_DIR}/kmax-revb-sdcard.dtb ${BINARIES_DIR}/kmax.dtb

BOARD_DIR="$(dirname $0)"
GENIMAGE_CFG="${BOARD_DIR}/genimage-revb.cfg"
GENIMAGE_TMP="${BUILD_DIR}/genimage-revb.tmp"


rm -rf "${GENIMAGE_TMP}"

genimage                           \
	--rootpath "${TARGET_DIR}"     \
	--tmppath "${GENIMAGE_TMP}"    \
	--inputpath "${BINARIES_DIR}"  \
	--outputpath "${BINARIES_DIR}" \
	--config "${GENIMAGE_CFG}"

#cp ./board/kaleao/set.bin ${BINARIES_DIR}/
#dd if=${BINARIES_DIR}/set.bin of=${BINARIES_DIR}/sdcard.img seek=1 

#cp ./board/kaleao/set.bin.revb-unsec ${BINARIES_DIR}/
cp ./kaleao/system/u-boot/kaleao/output/revb-unsec.set.bin ${BINARIES_DIR}/set.bin.revb-unsec
cp ${BINARIES_DIR}/kmax-revb-sdcard.dtb ${BINARIES_DIR}/kmax.dtb

BOARD_DIR="$(dirname $0)"
GENIMAGE_CFG="${BOARD_DIR}/genimage-revb-unsec.cfg"
GENIMAGE_TMP="${BUILD_DIR}/genimage-revb-unsec.tmp"


rm -rf "${GENIMAGE_TMP}"

genimage                           \
	--rootpath "${TARGET_DIR}"     \
	--tmppath "${GENIMAGE_TMP}"    \
	--inputpath "${BINARIES_DIR}"  \
	--outputpath "${BINARIES_DIR}" \
	--config "${GENIMAGE_CFG}"

#cp ./board/kaleao/set.bin ${BINARIES_DIR}/
#dd if=${BINARIES_DIR}/set.bin of=${BINARIES_DIR}/sdcard.img seek=1 
