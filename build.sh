<<<<<<< HEAD
#
# Copyright © 2017, "lordarcadius" <vipuljha08@gmail.com>
# Copyright © 2017, "arn4v"
#
# This software is licensed under the terms of the GNU General Public
# License version 2, as published by the Free Software Foundation, and
# may be copied, distributed, and modified under those terms.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
# GNU General Public License for more details.
#
# Please maintain this if you use this script or any part of it

# Color Codes
Black='\e[0;30m'        # Black
Red='\e[0;31m'          # Red
Green='\e[0;32m'        # Green
Yellow='\e[0;33m'       # Yellow
Blue='\e[0;34m'         # Blue
Purple='\e[0;35m'       # Purple
Cyan='\e[0;36m'         # Cyan
White='\e[0;37m'        # White


echo -e "$White***********************************************"
echo "         Compiling ElectraBlue Kernel             "
echo -e "***********************************************$nocol"

LC_ALL=C date +%Y-%m-%d
kernel_dir=$PWD
build=$kernel_dir/out
export CROSS_COMPILE="/home/lordarcadius/aarch64-linaro-linux-android/bin/aarch64-linaro-linux-android-"
kernel="ElectraBlue"
version="3.0"
vendor="xiaomi"
device="mido"
zip=zip
date=`date +"%Y%m%d-%H%M"`
config=mido_defconfig
kerneltype="Image.gz-dtb"
jobcount="-j$(grep -c ^processor /proc/cpuinfo)"
#modules_dir=$kernel_dir/"$zip"/system/lib/modules
modules_dir=$kernel_dir/"$zip"/modules
zip_name="$kernel"-"$version"-"$date"-"$device".zip
export KBUILD_BUILD_USER=LordArcadius
export KBUILD_BUILD_HOST=DroidBox

echo "Checking for build..."
if [ -d arch/arm64/boot/"$kerneltype" ]; then
	read -p "Previous build found, clean working directory..(y/n)? : " cchoice
	case "$cchoice" in
		y|Y )
			rm -rf out
			mkdir out
			rm -rf "$zip"/modules
			mkdir "$zip"/modules
			export ARCH=arm64
			make clean && make mrproper
			echo "Working directory cleaned...";;
		n|N )
			exit 0;;
		* )
			echo "Invalid...";;
	esac
	read -p "Begin build now..(y/n)? : " dchoice
	case "$dchoice" in
		y|Y)
			make "$config"
			make "$jobcount"
			exit 0;;

		n|N )
			exit 0;;
		* )
			echo "Invalid...";;
	esac
BUILD_END=$(date +"%s")
DIFF=$(($BUILD_END - $BUILD_START))
echo -e "$Green Build completed in $(($DIFF / 60)) minute(s) and $(($DIFF % 60)) seconds.$nocol"
fi
echo "Extracting files..."
if [ -f arch/arm64/boot/"$kerneltype" ]; then
	cp arch/arm64/boot/"$kerneltype" "$zip"/"$kerneltype"
#        mkdir -p zip/modules/pronto
#	cp drivers/staging/prima/wlan.ko zip/modules/pronto/pronto_wlan.ko
	find . -name '*.ko' -exec cp {} $modules_dir/ \;
	"$CROSS_COMPILE"strip --strip-unneeded "$zip"/modules/*.ko &> /dev/null
        mkdir -p zip/modules/pronto/
        mv zip/modules/wlan.ko zip/modules/pronto/pronto_wlan.ko
else
	echo "Nothing has been made..."
	read -p "Clean working directory..(y/n)? : " achoice
	case "$achoice" in
		y|Y )
                        rm -rf out
                        mkdir out
                        rm -rf "$zip"/modules
                        mkdir "$zip"/modules
			export ARCH=arm64
                        make clean && make mrproper
                        echo "Working directory cleaned...";;
		n|N )
			exit 0;;
		* )
			echo "Invalid...";;
	esac
	read -p "Begin build now..(y/n)? : " bchoice
	case "$bchoice" in
		y|Y)
			make "$config"
			make "$jobcount"
			exit 0;;
		n|N )
			exit 0;;
		* )
			echo "Invalid...";;
	esac
fi

echo "Zipping..."
if [ -f "$zip"/"$kerneltype" ]; then
	cd "$zip"
	zip -r ../$zip_name .
	mv ../$zip_name $build
	rm "$kerneltype"
	cd ..
	rm -rf arch/arm64/boot/"$kerneltype"
	echo "Generating changelog..."
        git --no-pager log --pretty=oneline --abbrev-commit 63940662c5bd707159770d06b4db10f6ee8b73af..HEAD > zip/changelog.txt
        paste zip/changelog.txt
        #rm zip/changelog.txt
	export outdir=""$build""
        export out=""$build""
        export OUT=""$build""
	echo "$BluePackage complete: "$build"/"$zip_name"$nocol"
	exit 0;
else
	echo "No $kerneltype found..."
	exit 0;
fi
# Export script by Savoca
# Thank You Savoca!
=======
#!/bin/bash
echo "WAFF WAFF WAFF WAFF WAFF WAFF WAFF WAFF"
echo "########### LOBO UTILIDADES ###########"
echo "WAFF WAFF WAFF WAFF WAFF WAFF WAFF WAFF"
echo ""
#echo "> Copiando archivo de configuracion"
#cp ../boot_miui_official/msm8940_defconfig .config
#echo "> Copiando ramdisk a arch/arm64/boot"
#cp ../generated_boot_img/initramfs.cpio.gz arch/arm64/boot/boot.img-ramdisk.cpio.gz
echo "> Abriendo archivo de configuracion"
CFLAGS="-mtune=cortex-a53 -march=armv8-a -mcpu=cortex-a53 -O2" ARCH=arm64 CROSS_COMPILE=../aarch64-linux-android-4.9/bin/aarch64-linux-android- make menuconfig
echo "> Iniciando compilacion del kernel usando .config"
CFLAGS="-mtune=cortex-a53 -march=armv8-a -mcpu=cortex-a53 -O2" ARCH=arm64 CROSS_COMPILE=../aarch64-linux-android-4.9/bin/aarch64-linux-android- make -j4
echo ""
#echo "> Eliminando boot.img viejo"
#rm arch/arm64/boot/boot.img
#echo "> EMPAQUETANDO Kernel..."
#./../mkbootimg/mkbootimg --kernel arch/arm64/boot/Image.gz-dtb --ramdisk arch/arm64/boot/boot.img-ramdisk.cpio.gz --cmdline 'console=ttyHSL0,115200,n8 androidboot.console=ttyHSL0 androidboot.hardware=qcom msm_rtb.filter=0x237 ehci-hcd.park=3 lpm_levels.sleep_disabled=1 androidboot.bootdevice=7824900.sdhci earlycon=msm_hsl_uart,0x78B0000 buildvariant=eng androidboot.selinux=permissive enforcing=0' --base 0x80000000 -o arch/arm64/boot/boot.img
#echo "> ELIMINANDO MODULOS ANTERIORES"
#rm -rf /media/psf/Home/Desktop/kmodules
#mkdir /media/psf/Home/Desktop/kmodules
#echo "> COPIANDO MODULOS A /media/psf/Home/Desktop/kmodules"
#find . -type f -name "*.ko" -exec cp -fv {} /media/psf/Home/Desktop/kmodules/. \;
#echo "> COPIANDO boot.img a /media/psf/Home/Desktop/"
#cp arch/arm64/boot/boot.img /media/psf/Home/Desktop/.

./../AnyKernel2/build.sh
echo "WAAAAAAAF IS DONE"
>>>>>>> 8f5d770414a10b7c363c32d12f188bd16f7b6f24
