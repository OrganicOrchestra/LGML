#!/bin/bash


#https
#sudo apt-get -y install libcurl4-gnutls-dev

NATIVE_ARCH=`dpkg --print-architecture`
if [ -z ${LGML_TARGET_ARCH+x} ]; then LGML_TARGET_ARCH="$NATIVE_ARCH"; fi
if [ "$LGML_TARGET_ARCH" != "$NATIVE_ARCH" ]; then echo "adding foreing arch $LGML_TARGET_ARCH"; dpkg --add-architecture $LGML_TARGET_ARCH;apt-get -qq update; fi
echo "arch is set to '$LGML_TARGET_ARCH'"
# rubberband
apt-get -y --force-yes install librubberband-dev:$LGML_TARGET_ARCH


