# MKS-Robin-Nano
## MKS Robin Nano V2.0
- Support uart mode and spi mode.
- Support SENSORLESS_HOMING function.
- Use the box header connector, more solid and stable.
- Onboard 4k EEPROM.
- Firmware is based on Marlin 2.x.
- Transplanted the cool LVGL graphics library to marlin.
- Most of the machine parameters are opened on the touch screen, users can easily configure, no need to modify the configuration file.

## Related tutorials and Notice
- V1.2 user Manual. [click here](https://github.com/makerbase-mks/MKS-Robin-Nano/wiki/MKS-Robin-Nano-User-Manual)
- V2.0 user manual. [click here](https://github.com/makerbase-mks/MKS-Robin-Nano/wiki/MKS-Robin-Nano-V2.0-User-Manual)
- Firmware v2.0.0 release. [click here](https://www.youtube.com/watch?v=Nv3Zgs8XUt4)
- Thank you very much for Miguel Angel Romero Sanchez(3D Maker ES)'s awesome video for MKS Robin Nano. [click here](https://www.youtube.com/watch?v=ucmC3b5-Wj4&feature=share&fbclid=IwAR2TWLEQ-a9ndR3HesDfjPQxTEMIGboHutLvQPGv8t6IWoZZtjgn14PVID4)
- Thank you very much for 3DWork's introduction about MKS Robin Nano and Nano firmware. [click here](https://3dwork.io/mks-robin-nano-v1-2-analyzing-the-new-makerbase-32bit-boards/?fbclid=IwAR2QDFUOCh8_rZrIKsDni5x6sH0ezoaqEYq9zsHAT5BPOriKch0kDYLzZ0g&tdsourcetag=s_pctim_aiomsg)
- Welcome to follow us on Facebook to learn about the company's latest developments. [click here](https://www.facebook.com/Makerbase.mks/)

# Firmware
There are seval firmwares fit Robin Nano:
- [Marlin2.0-For-Robin-Nano](https://github.com/makerbase-mks/Mks-Robin-Nano-Marlin2.0-Firmware)
This repository is based on Marlin2.0.x, added the [LittlevGL](https://github.com/littlevgl/lvgl), the colourful GUI. It is developed on PlatformIO, we hope more and more developers will participate the development of this repository.
- [MKS-Robin-Nano](https://github.com/makerbase-mks/MKS-Robin-Nano-Firmware)
This is the factory version of the MKS Robin Nano, the performance is most stable, but as the development IDE needs to pay, it is not suitable for individual users to secondary development
- [Marlin2.0-For-Robin-Nano-old-version](https://github.com/makerbase-mks/Mks-Robin-Nano-Marlin2.0-Firmware-old-version.git)
This version is also based on marlin2.0, but the version is relatively old, and the LCD display only supports marlin's native display, no cool GUI. Later, it will be slowly replaced by the version with cool GUI.

---

## The firmware versions mentioned below are for the factory version ##

## Nano-firmware-v2.x.x
- Parameters are set directly through the screen
- After the parameter is set, it will work directly without restarting
- More suitable for DIY users

## Nano-firmware-v1.x.x
- Parameters are configured using config.txt
- Need to power off and restart for the parameters to be effective
- More suitable for 3D printer manufacturers and users who often do not modify parameters

## Firmware version description
## v2.x.x

### v2.0.3.2
- Increase the pause position can be configured online.

### v2.0.3.1
- Fix the display problem of "More" interface
- Fix the problem of print pause when awakening

### v2.0.3
- Add quick pause function
- Support mks ups module

### v2.0.2
- Fix the problem that the background color and font color of the dialog box are displayed incorrectly
- Fix the problem that the leveling parameters of corexy models do not work
- Compatible with BLTOUCH3.1

### v2.0.1
- Add Babystep function
- Add Z_SAFE_HOMING function

### v2.0.0
- DIY style initial version.

## v1.x.x
### v1.0.4
 - Fixed the issue of maximum temperature not reporting errors.
 - Fixed the problem that the nozzle is always heated during SD card read error.

### v1.0.3:
- The robin_nano35V_1.0.3 firmware is upgraded from robin_nano_v1.0.2
- Fix bug:When the temperature protection time is too long, the M303 cannot be adjusted.
- Add two UI styles, retro and simple.
- When the WIFI function is not enabled in the configuration file, the WIFI button in the interface is not displayed.
- The information about the motherboard inside can be modified by the configuration file.
- When there is a pause or power failure, the printing should be pre-extruded for a period of time. Otherwise, there may be a shortage of material and a fault. The extrusion length can be set in the configuration document.
- Add two in one out function, configurable.
- Increase the screen flip 180 degrees in the configuration file (bootloader can be flipped).
- Add wifi scan list function, configurable.
- After the printing is completed, a dialog box will pop up to show that the printing has been completed. How long does it take? There are "OK" and "Reprint once" buttons.
- Compatible with 3D_TOUCH function.
- Increase screen wake-up function, configurable.
- Modify the bootloader, fix the failure to use the TB67S109 driver PWC shutdown.
- Open pulse hold time parameter.
- Fixed the problem that when using 3Dtouch to do automatic leveling, the power failure will continue to print.
- Fix bug:When the material detection is triggered by a low level, after the material is cut off, the paper will resume printing after the material is not in use, and the printing will be stopped after a period of time, without prompting to install the consumables first.
- Fix bug:Set to two in and one out, the extrusion head is selected as the extrusion head 2 at the extrusion interface, and then preheated at the preheating interface, the preheating temperature cannot be set.
- Fix bug:Remove the break detection switch setting in the switch type setting.
- After the sleep mode is enabled, the preview model image in the print interface after hibernation is not displayed;
- After the MKS PWC module is connected, the delay is printed for 3 minutes and then turned off to prevent plugging (time configurable).
- Fix bug:When the z-axis returns to zero maximum (the first z-axis limit is connected to zmax), when the double z limit is enabled, the z2 limit is connected to the xmax interface.When z returns to zero, when the z1 limit is triggered, the two z axes will stop at the same time.
- Adding a pause position can be selected as an invalid item.
- Modify the file name to exceed 30 bytes, and stop the problem that the Z axis is going back to the zero direction.

## Note
- Thank you for using MKS products. If you have any questions during use, please contact us in time and we will work with you to solve it.
- For more product dynamic information and tutorial materials, you can always follow MKS's Facebook and GitHub and YouTube. Thank you!
![](https://github.com/makerbase-mks/MKS-Robin-Nano/blob/master/hardware/Image/MKS_FGA.png)
