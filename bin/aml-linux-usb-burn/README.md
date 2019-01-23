# Linux version of Amlogic USB Burning Tool

This is just "copy" of files that can be found on Amlogic Openlinux [website][amlTools].

# Installation
Create new u-dev rule for Amlogic devices (in /etc/udev/rules.d). My file is called 70-persistent-usb-amlogic.rules
Content of file is:
```
SUBSYSTEM=="usb", ENV{DEVTYPE}=="usb_device", ATTR{idVendor}=="1b8e", ATTR{idProduct}=="c003", MODE:="0666", SYMLINK+="worldcup"
```

When you created your rule either reload udev rules or reboot your machine. Make sure that root folder of this repository is in your PATH variable (I simply put it inside my ~/bin folder) so I can call it from anywhere without specifying path.

And this tool depends on `libusb` and `libusb-compat`, you must install those tools first.

#  How to use
Connect your device and put it to USB burning mode, open terminal and navigate to folder where your aml_upgrade_package.img is and issue command:
```
aml-flash --img=aml_upgrade_package.img --soc=gxl --wipe --reset=n --parts=all
```
For more options, just issue aml-flash command.
__soc__ paremeter can be gxl (S905, S905X and S912), axg (A113 audio SoC), txlx (TV SoC - T962), m8 (S802, S805 and S812)
I tested this tool with sucess on S812, S905, S905X and S912.

#  Note
This tool is for x64 Linux only. File aml_image_v2_packer is 32bit but update binary (inside tools folder) is 64bit so...

   [amlTools]: <http://openlinux.amlogic.com:8000/download/A113/Tool/flash-tool-v4.7/flash-tool>

