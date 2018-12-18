#!/bin/sh
echo "Start cleaning useless contents..."

# sdfuse
rm -rf ./sd_fuse/image_secure/*

echo "Start packaging..."
make clean distclean
rm -f tags
mv .git/ ../
tar -jcf ../avl7420_uboot.tar.bz2 *
mv ../.git/ .

echo "Finished!"
