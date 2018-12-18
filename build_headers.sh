#!/bin/bash

### RUN THIS ON A BUILT KERNEL ###

echo 'Building kernel headers...'

DIR=$(dirname "$0")
cd "$DIR"

OUTDIR=./ubuntu/kernel_headers

VERS=$(ls ./ubuntu/kernel_modules/lib/modules)

rm -rf "$OUTDIR"
./kaleao/system/kernel_headers/headers.sh \
    ./kaleao/system/exynos7420_kernel_4.4 "$OUTDIR"/usr/src/"$VERS"
if [ "$?" != 0 ]; then
        exit 1
fi

tar -xf ./kaleao/system/kernel_headers/scripts.tar.xz \
    -C "$OUTDIR"/usr/src/"$VERS"



