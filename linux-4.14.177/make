#!/bin/sh

usage()
{
	echo "***********************************************"
	echo "USAGE"
	echo "./make [object]"
	echo "DESCIPTION"
	echo "object: config/menuconfig/kernel/clean/startup"
	echo "***********************************************"
}

str_to_lower()
{
	echo $1 | tr '[A-Z]' '[a-z]'
}

str=$(str_to_lower $1)
if [ "$str" == "config" ]; then
	cp default.config .config 
elif [ "$str" == "menuconfig" ]; then
	make ARCH=arm64 CROSS_COMPILE=aarch64-linux-gnu- menuconfig
elif [ "$str" == "kernel" ]; then
	make ARCH=arm64 CROSS_COMPILE=aarch64-linux-gnu- -j8
elif [ "$str" == "clean" ]; then
	make ARCH=arm64 CROSS_COMPILE=aarch64-linux-gnu- distclean
elif [ "$str" == "startup" ]; then
	qemu-system-aarch64 -machine virt -cpu cortex-a53 -machine type=virt -nographic -m 4096 -smp 4 -kernel arch/arm64/boot/Image --append "rdinit=/linuxrc console=ttyAMA0"
elif [ "$str" == "gdb" ]; then
	qemu-system-aarch64 -s -S -machine virt -cpu cortex-a53 -machine type=virt -nographic -m 1024 -smp 2 -kernel arch/arm64/boot/Image --append "rdinit=/linuxrc console=ttyAMA0"
elif [ "$str" == "start" ]; then
	qemu-system-aarch64 -machine virt -cpu cortex-a53 -machine type=virt -m 4096 -smp 2 -kernel arch/arm64/boot/Image --append "rdinit=/linuxrc root=/dev/vda rw console=ttyAMA0 loglevel=8"  -nographic --fsdev local,id=kmod_dev,path=$PWD/kmodules,security_model=none -device virtio-9p-device,fsdev=kmod_dev,mount_tag=kmod_mount
else
	usage
	exit 1
fi

