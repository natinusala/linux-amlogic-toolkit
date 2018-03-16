#!/bin/sh
#

if [ "$1" = "-f" ]; then
    forcewrite=yes
    shift
fi

initrd=${1:-initrd.img}
ramdisk=${2:-ramdisk}

if [ ! -d $ramdisk ]; then
    echo "$ramdisk does not exist."
    exit 1
fi

if [ -f $initrd -a -z "$forcewrite" ]; then
    echo "$initrd already exist."
    exit 1
fi

( cd $ramdisk; find | sort | cpio --quiet -o -H newc ) | gzip > $initrd

