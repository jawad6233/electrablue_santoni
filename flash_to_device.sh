#!/bin/bash
#echo "> Copiando modulos a la sdcard"
#adb push /media/psf/Home/Desktop/kmodules /sdcard
echo "> Reiniciando en modo fastboot"
adb reboot bootloader
echo "> Flasheando boot.img al dispositivo..."
fastboot flash boot arch/arm64/boot/boot.img
echo "> Reiniciando el dispotivo..."
fastboot reboot
