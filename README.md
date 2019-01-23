# Linux AMLogic Toolkit

Allows to unpack and repack AMLogic Android images on Linux systems without using the Customization Tool - works for Android 7.

# Features
* Unpack and repack any image
* Mount and edit `system` partition
* Unpack and repack `logo` partition (for bootup and upgrading logos)
* Unpack and repack `boot` image and `initrd` ramdisk
* Flash the image directly to a device without repacking it (faster than using the USB Burning Tool)
* Works for Android 7
* No need to unpack the image each time you want to use the tool

# What it cannot do (yet)
* Edit other partitions of the image such as `recovery` (you can still replace the `PARTITION` files by hand)

# Dependencies
* `zlib1g-dev` for `simg2img` and `img2simg`
* `libblkid-dev` for `abootimg` (unpacking and repacking boot image)
* the `i386` packages if needed (for the logo unpacking / repacking binary)

# How to use it
* Clone or download this repository
* Install the dependencies
* Move to the directory of the repository, and **stay there**
* *(first time, or after a cleanup)* Run `./bin/build` to build the required tools
* *(when editing a new image file)* Run `./bin/unpack input.img` to unpack `input.img`
* The result is :
    * `output/image` : raw image files (`PARTITION` files)
    * `output/system` : system partition files
    * `output/logo` : logo partition files
    * `output/boot` : boot partition files
* From now on you can edit the files of the `output` directory
    * Note that those files will be overwritten when repacking :
        * `output/image/system.PARTITION`
        * `output/image/boot.PARTITION`
        * `output/image/logo.PARTITION`
        * `output/boot/initrd.img` if using `./bin/extract_initrd`
    * If you happen to loose the `output/system` mounting point (after a reboot for instance), just run `./bin/remount` to mount it again
    * On the other hand, you can unmount the system partition using `./bin/unmount`
    * If you want to extract the `initrd` ramdisk, use the `./bin/extract_initrd` and `./bin/recreate_initrd` scripts (output in `output/initrd`)
* **Be careful :**
    * Don't break everything by chmod'ing the whole `output/system` folder, because it will be replicated in the image and it won't boot !
    * Don't rename the files in `output/boot.img`
    * If you extract and recreate the `initrd` ramdisk, its size will change and it will most likely break the boot image. To fix this, edit the `bootimg.cfg` file in `output/boot` to replicate the change in size (you can repack the image, let it fail and read the logs to see the new size).
* When you have finished editing the files, run `./bin/repack output.img` to repack the image to `output.img`
* Additionnaly, you can use `./bin/flash` to flash the image to a device through USB (you will need the udev rule, see https://github.com/Stane1983/aml-linux-usb-burn)
    * The device type (`gxl`) is hardcoded into the flashing script, edit it if you're not using S905, S905X or S919
* Done !

# Troubleshooting
* If you have a `file not found` error when trying to unpack and repack the logo partition, install the `i386` libraries by following the accepted answer of this post : https://unix.stackexchange.com/questions/13391/getting-not-found-message-when-running-a-32-bit-binary-on-a-64-bit-system

# Credits

* Thanks to Magendanz and adg for the unpacking and repacking method
* https://github.com/khadas/utils for the `aml_image_v2_packer` and `logo_img_packer` binaries
* https://github.com/anestisb/android-simg2img for the `simg2img` tool
* https://github.com/ggrandou/abootimg for the `abootimg` tool
* https://github.com/Stane1983/aml-linux-usb-burn for the flashing tool



