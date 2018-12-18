#!/bin/bash

if [ "$#" -ne 3 ]; then
	echo -e 'Usage: ./build_deb.sh <package name> <pdesc file (without .pdesc)> mode'
	exit 0
fi

PDESC="kaleao/updates/packages/$3/$1/$2.pdesc"

if [ ! -f $PDESC ]; then
	echo "Package .pdesc file \"$PDESC\" was not found!"
	exit 1
fi

source $PDESC

UPDATES_DIR=kaleao/updates
TEMP_DIR=kaleao/temp/$3_$1_$2
PKG_DIR=kaleao/updates/packages/$3/$1

if [ "`which dpkg-deb`" = "" ]; then
	echo "dpkg-deb not present! Please install"
fi

#Init helping dirs
mkdir -p $UPDATES_DIR/debs/$3

#Create debian dir
rm -rf $TEMP_DIR
DEB_DIR=$TEMP_DIR/$PKG_NAME/DEBIAN
mkdir -p $DEB_DIR

#Create control file
echo "Package: $PKG_NAME" >> $DEB_DIR/control
echo "Version: $PKG_VERSION" >> $DEB_DIR/control
echo "Priority: optional" >> $DEB_DIR/control
echo "Architecture: all" >>  $DEB_DIR/control

if [ "$PKG_DEPENDS" != "" ]; then
	echo "Depends: $PKG_DEPENDS" >> $DEB_DIR/control
fi

echo "Maintainer: $PKG_MAINTAINER" >> $DEB_DIR/control
echo "Description: $PKG_DESC" >> $DEB_DIR/control

#Create overlay
for ((i=0;i<${#SRC_FILES[@]};i++))
do
	mkdir -p `dirname $TEMP_DIR/$PKG_NAME/${DST_FILES[$i]}`
	cp -r ${SRC_FILES[$i]} $TEMP_DIR/$PKG_NAME/${DST_FILES[$i]}
done

if [ "${RECOVERY}" == "yes" ]; then
	for ((i=0;i<${#SRC_FILES[@]};i++))
	do
		mkdir -p `dirname $TEMP_DIR/$PKG_NAME/${DST_FILES_RECOVERY[$i]}`
		cp -r ${SRC_FILES_RECOVERY[$i]} $TEMP_DIR/$PKG_NAME/${DST_FILES_RECOVERY[$i]}
	done
fi


# Build-target specific files
declare -n SRC_FILES_SP="SRC_FILES_${BUILD_TARGET}"
declare -n DST_FILES_SP="DST_FILES_${BUILD_TARGET}"

if [ "$SRC_FILES_SP" ]; then
	for ((i=0;i<${#SRC_FILES_SP[@]};i++))
	do
		mkdir -p `dirname $TEMP_DIR/$PKG_NAME/${DST_FILES_SP[$i]}`
		cp -r ${SRC_FILES_SP[$i]} $TEMP_DIR/$PKG_NAME/${DST_FILES_SP[$i]}
	done
fi

if [ -f $PKG_DIR/$2.postinst ]; then
	cp -f $PKG_DIR/$2.postinst $DEB_DIR/postinst
	chmod +x $DEB_DIR/postinst
fi

if [ -f $PKG_DIR/$2.prerm  ]; then
	cp -f $PKG_DIR/$2.prerm $DEB_DIR/prerm
	chmod +x $DEB_DIR/prerm
fi

MYDIR=`pwd`

cd $TEMP_DIR
dpkg-deb -Zgzip --build $PKG_NAME
if [ "$?" != 0 ]; then
        prerr "Failed to build deb package "$PKG_NAME
        cd $MYDIR
        exit 1
fi

cd $MYDIR

if [ ! -f "$TEMP_DIR/$PKG_NAME.deb" ]; then
        prerr "Failed to build deb package "$PKG_NAME
        exit 1
fi

cp -f $TEMP_DIR/$PKG_NAME.deb $UPDATES_DIR/debs/$3/$PKG_NAME.deb

rm -rf $TEMP_DIR
