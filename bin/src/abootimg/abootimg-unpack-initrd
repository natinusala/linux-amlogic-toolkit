#!/bin/sh
#

initrd=${1:-initrd.img}
ramdisk=${2:-ramdisk}

if [ ! -f $initrd ]; then
    echo "$initrd does not exist."
    exit 1
fi

if [ -d $ramdisk ]; then
    echo "$ramdisk already exists."
    exit 1
fi

mkdir -p $ramdisk

zcat $initrd | ( cd $ramdisk; cpio -i )

