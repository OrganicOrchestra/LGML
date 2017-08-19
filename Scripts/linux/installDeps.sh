#!/bin/bash


#https
#sudo apt-get -y install libcurl4-gnutls-dev
if [ -z ${LGML_TARGET_ARCH+x} ]; then LGML_TARGET_ARCH=`arch`; else echo "arch is set to '$LGML_TARGET_ARCH'"; fi
# rubberband
apt-get -y --force-yes install librubberband-dev:$LGML_TARGET_ARCH


