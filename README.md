# Linux AMLogic Toolkit

Allows to unpack and repack AMLogic Android images on Linux systems without using the Customization Tool - works for Android 7.

# Features
* Unpack and repack any image
* Mount and edit `system` partition
* Unpack and repack `logo` partition (for bootup and upgrading logos)
* Unpack and repack `boot` image
* Works for Android 7
* No need to unpack the image each time you want to use the tool

# What it cannot do (yet)
* Edit other partitions of the image such as `recovery` (you can still replace the `PARTITION` files by hand)

# Dependencies
* `libblkid-dev` for unpacking and repacking `boot.img`

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
* From now on you can edit the files of both the `image` and `system` directories
    * Note that the `system.PARTITION`, `boot.PARTITION`, `logo.PARTITION` files will be replaced when repacking
    * If you happen to loose the `output/system` mounting point (after a reboot for instance), just run `./bin/remount` to mount it again
    * On the other hand, you can unmount the system partition using `./bin/unmount`
**Be careful :**
    * Don't break everything by chmod'ing the whole `output/system` folder, because it will be replicated in the image and it won't boot !
    * Don't rename the files in `output/boot.img`
* When you have finished editing the files, run `./bin/repack output.img` to repack the image to `output.img`
* Done !

# Credits

* Thanks to Magendanz and adg for the unpacking and repacking method
* https://github.com/khadas/utils for the `aml_image_v2_packer` and `logo_img_packer` binaries
* https://github.com/anestisb/android-simg2img for the `simg2img` tool
* https://github.com/ggrandou/abootimg for the `abootimg` tool



