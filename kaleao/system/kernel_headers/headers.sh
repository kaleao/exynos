#!/bin/bash

[ $# -lt 2 ] && {
    echo "Usage:"
    echo "    $0 kernel_dir output_dir"
    exit 1
}

mkdir -p "$2"
TARGET_DIR=$(readlink -f "$2")

cd "$1"

cp Makefile "$TARGET_DIR"/
mkdir "$TARGET_DIR"/kernel
cp kernel/Makefile "$TARGET_DIR"/kernel
cp .config "$TARGET_DIR"

mkdir "$TARGET_DIR"/include

for i in acpi asm-generic clocksource config crypto drm generated keys kvm linux math-emu \
    media net pcmcia rdma scsi soc sound trace uapi video xen; do
  cp -a include/${i} "$TARGET_DIR/include/"
done

# copy arch includes for external modules
mkdir -p "$TARGET_DIR"/arch/arm64
mkdir -p "$TARGET_DIR"/arch/arm
cp -a arch/arm64/include "$TARGET_DIR"/arch/arm64
cp -a arch/arm/include "$TARGET_DIR"/arch/arm

# copy files necessary for later builds, like nvidia and vmware
cp Module.symvers "$TARGET_DIR"
## cp -a scripts "$TARGET_DIR"

# fix permissions on scripts dir
## chmod og-w -R "$TARGET_DIR"/scripts
mkdir "$TARGET_DIR"/.tmp_versions

mkdir -p "$TARGET_DIR"/arch/arm64/kernel

cp arch/arm64/Makefile "$TARGET_DIR"/arch/arm64/
cp arch/arm/Makefile "$TARGET_DIR"/arch/arm/

cp arch/arm64/kernel/asm-offsets.s "$TARGET_DIR"/arch/arm64/kernel/

# add docbook makefile
mkdir -p "$TARGET_DIR"/Documentation/DocBook
cp Documentation/DocBook/Makefile "$TARGET_DIR"/Documentation/DocBook/Makefile

# add dm headers
mkdir -p "$TARGET_DIR"/drivers/md
cp drivers/md/*.h "$TARGET_DIR"/drivers/md

# add inotify.h
mkdir -p "$TARGET_DIR"/include/linux
cp include/linux/inotify.h "$TARGET_DIR"/include/linux/

# add wireless headers
mkdir -p "$TARGET_DIR"/net/mac80211/
cp net/mac80211/*.h "$TARGET_DIR"/net/mac80211/

# add dvb headers for external modules
mkdir -p "$TARGET_DIR"/drivers/media/dvb-core
cp drivers/media/dvb-core/*.h "$TARGET_DIR"/drivers/media/dvb-core/

# add dvb headers for http://mcentral.de/hg/~mrec/em28xx-new
mkdir -p "$TARGET_DIR"/drivers/media/dvb-frontends/
cp drivers/media/dvb-frontends/lgdt330x.h "$TARGET_DIR"/drivers/media/dvb-frontends/
mkdir -p "$TARGET_DIR"/drivers/media/i2c/
cp drivers/media/i2c/msp3400-driver.h "$TARGET_DIR"/drivers/media/i2c/

# add dvb headers
mkdir -p "$TARGET_DIR"/drivers/media/usb/dvb-usb
cp drivers/media/usb/dvb-usb/*.h "$TARGET_DIR"/drivers/media/usb/dvb-usb/
mkdir -p "$TARGET_DIR"/drivers/media/dvb-frontends
cp drivers/media/dvb-frontends/*.h "$TARGET_DIR"/drivers/media/dvb-frontends/
mkdir -p "$TARGET_DIR"/drivers/media/tuners
cp drivers/media/tuners/*.h "$TARGET_DIR"/drivers/media/tuners/

# add xfs and shmem for aufs building
mkdir -p "$TARGET_DIR"/fs/xfs
mkdir -p "$TARGET_DIR"/mm

# copy in Kconfig files
for i in $(find . -name "Kconfig*"); do
  mkdir -p "$TARGET_DIR"/`echo ${i} | sed 's|/Kconfig.*||'`
  cp ${i} "$TARGET_DIR/${i}"
done

# chown -R root.root "$TARGET_DIR"
find "$TARGET_DIR" -type d -exec chmod 755 {} \;

# strip scripts directory
## find "$TARGET_DIR"/scripts -type f -perm -u+w 2>/dev/null | while read binary ; do
##   case "$(file -bi "${binary}")" in
##     *application/x-sharedlib*) # Libraries (.so)
##       /usr/bin/strip ${STRIP_SHARED} "${binary}";;
##     *application/x-archive*) # Libraries (.a)
##       /usr/bin/strip ${STRIP_STATIC} "${binary}";;
##     *application/x-executable*) # Binaries
##       /usr/bin/strip ${STRIP_BINARIES} "${binary}";;
##   esac
## done

# remove unneeded architectures
rm -rf "$TARGET_DIR"/arch/{alpha,arc,arm26,avr32,blackfin,c6x,cris,frv,h8300,hexagon,ia64,m32r,m68k,m68knommu,metag,mips,microblaze,mn10300,nios2,openrisc,parisc,powerpc,ppc,s390,score,sh,sh64,sparc,sparc64,tile,unicore32,um,v850,x86,xtensa}

