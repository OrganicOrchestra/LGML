#!/bin/sh
export ARCH_FLAGS_PI3='-mcpu=cortex-a53 -mfpu=neon-fp-armv8 -mfloat-abi=hard'
#export CROSS_ARCH="armhf"
#export PKG_CONFIG_PATH='/usr/lib/arm-linux-gnueabihf/pkgconfig'
sh Scripts/linux/installDevDeps.sh
