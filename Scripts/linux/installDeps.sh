#!/bin/bash


#https
#sudo apt-get -y install libcurl4-gnutls-dev

NATIVE_CPU=`dpkg --print-architecture`
if [ -z ${TARGET_CPU+x} ]; then TARGET_CPU="$NATIVE_CPU"; fi
if [ "$TARGET_CPU" != "$NATIVE_CPU" ]; then echo "adding foreing arch $TARGET_CPU"; dpkg --add-architecture $TARGET_CPU;apt-get -qq update; fi
echo "arch is set to '$TARGET_CPU'"
# rubberband
apt-get -y --assume-yes install librubberband-dev:$TARGET_CPU
# for dns utility
apt-get -y --assume-yes install libavahi-compat-libdnssd-dev:$TARGET_CPU

