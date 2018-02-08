# Linux AMLogic Toolkit

Allows to unpack and repack AMLogic Android images on Linux systems without using the Customization Tool - works for Android 7.

# Features
* Unpack and repack any image
* Mount and edit `system` partition
* Unpack and repack `logo` partition (for bootup and upgrading logos)
* Works for Android 7
* No need to unpack the image each time you want to use the tool

# What it cannot do (yet)
* Edit other partitions of the image such as `recovery` or `boot` (you can still replace the `PARTITION` files by hand)

# How to use it
* Clone or download this repository
* Move to the directory of the repository, and **stay there**
* *(first time, or after a cleanup)* Run `./bin/build` to build the required tools
* *(when editing a new image file)* Run `./bin/unpack input.img` to unpack `input.img`
    * The image files can now be found in `output/image`, the system partition files in `output/system` and the logo images in `output/logo`
* From now on you can edit the files of both the `image` and `system` directories
    * Note that the `system.PARTITION` and `logo.PARTITION` files will be replaced when repacking
    * If you happen to loose the `output/system` mounting point (after a reboot for instance), just run `./bin/remount` to mount it again
    * On the other hand, you can unmount the system partition using `./bin/unmount`
* When you have finished editing the files, run `./bin/repack output.img` to repack the image to `output.img`
* Done !

# Credits

* Thanks to Magendanz and adg for the unpacking and repacking method
* https://github.com/khadas/utils for the `aml_image_v2_packer` and `logo_img_packer` binaries
* https://github.com/anestisb/android-simg2img for the `simg2img` tool
* https://www.cnx-software.com/2016/11/19/how-to-create-a-bootable-recovery-sd-card-for-amlogic-tv-boxes/ for the SD Card "burning" tool



