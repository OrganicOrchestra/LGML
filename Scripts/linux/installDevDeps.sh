# this script install JUCE in sibling directory of LGML
set +e # ignore apt update error
NATIVE_CPU=`dpkg --print-architecture`
if [ -z ${TARGET_CPU+x} ]; then TARGET_CPU="$NATIVE_CPU"; fi
if [ "$TARGET_CPU" != "$NATIVE_CPU" ]; then echo "adding foreing arch $TARGET_CPU"; dpkg --add-architecture $TARGET_CPU;apt-get -qq update; fi
echo "arch is set to '$TARGET_CPU'"

set -e # un-ignore apt update error

## these are devloper libs needed for JUCE,   not sure wich are needed in released version...
# from Makefile alsa freetype2 libcurl x11 xext xinerama
apt-get -y --assume-yes install libfreetype6-dev:$TARGET_CPU 
apt-get -y --assume-yes install libx11-dev:$TARGET_CPU
apt-get -y --assume-yes install libxinerama-dev:$TARGET_CPU
apt-get -y --assume-yes install libxrandr-dev:$TARGET_CPU
apt-get -y --assume-yes install libxcursor-dev:$TARGET_CPU
apt-get -y --assume-yes install mesa-common-dev:$TARGET_CPU
apt-get -y --assume-yes install libasound2-dev:$TARGET_CPU
apt-get -y --assume-yes install freeglut3-dev:$TARGET_CPU
apt-get -y --assume-yes install libxcomposite-dev:$TARGET_CPU
apt-get -y --assume-yes install libjack-dev:$TARGET_CPU
apt-get -y -q --assume-yes install libcurl4-openssl-dev:$TARGET_CPU
apt-get -y -q --assume-yes install ladspa-sdk:$TARGET_CPU

# libavahi-compat-libdnssd libfreetype6 libx11 libxinerama libxrandr libxcursor mesa-common libasound2 freeglut3 libxcomposite libjack libcurl4-openssl

apt-get -y -q --assume-yes install curl

SCRIPTPATH=`pwd`/$(dirname "$0") 
cd $SCRIPTPATH
cd ../../..

if [ ! -d "JUCE" ]; then
  curl -L https://github.com/julianstorer/JUCE/archive/master.tar.gz > JUCE.tar.gz
  tar -xf JUCE.tar.gz
  mv JUCE-master/ JUCE
fi

apt-get -y -q --assume-yes install python3
apt-get -y -q --assume-yes install python3-pip


# cd ;
# if [ ! -d "Dev/Projucer/linux" ]; then
#   cd 
#   ls

#   cd Dev/JUCE/extras/Projucer/Builds/LinuxMakefile/
#   make -j2
#   cd 
#   pwd
#   mkdir -p Dev/Projucer
#   mkdir -p Dev/Projucer/linux
#   mv Dev/JUCE/extras/Projucer/Builds/LinuxMakefile/build/Projucer Dev/Projucer/linux/
# fi


