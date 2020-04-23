#!/usr/bin/env bash

# this script install deps needed to compile LGML from source :
# ** /!\ /!\ /!\ /!\ if gcc < 4.8 c++14 may break the build /!\ /!\ /!\ /!\  (on ubuntu <16 for example)
# ** JUCE is downloaded in a sibling directory of LGML
# ** JUCE deps also
# this script support cross-compiling setting the CROSS_ARCH flag to armhf (see circleci config.yml)



set +e # ignore apt update error
NATIVE_ARCH=`dpkg --print-architecture`
if [ -z ${CROSS_ARCH+x} ]; then CROSS_ARCH="$NATIVE_ARCH"; fi
if [ "$CROSS_ARCH" != "$NATIVE_ARCH" ]; then echo "adding foreing arch $CROSS_ARCH"; dpkg --add-architecture $CROSS_ARCH;apt-get -qq update; fi
echo "arch is set to '$CROSS_ARCH'"

set -e # un-ignore apt update error

apt-get install -y --assume-yes  build-essential
echo "checking if gcc>4.9"
vlte () {
dpkg --compare-versions "$1" "lt" "$2"
}

vlte "4.9.0" `gcc -dumpversion` && echo "...ok" || VERSION_CHECK_ERROR="gcc version is too low, JUCE for c++14 support"

set +e

if [ -n "$VERSION_CHECK_ERROR" ]; then
  echo "$VERSION_CHECK_ERROR"
  # modifying gcc is likely to break things up
  echo `gcc -dumpversion`
  # exit 1
  ###add-apt-repository ppa:ubuntu-toolchain-r/test
  #echo "deb http://ftp.us.debian.org/debian unstable main contrib non-free" >> /etc/apt/sources.list.d/unstable.list
  #apt-get update && apt-get install -y --force-yes -t unstable gcc-5 g++-5 && update-alternatives --install /usr/bin/gcc gcc /usr/bin/gcc-5 60 --slave /usr/bin/g++ g++ /usr/bin/g++-5
fi

set -e


## these are devloper libs needed for JUCE,   not sure wich are needed in released version...
# from Makefile alsa freetype2 libcurl x11 xext xinerama
apt-get -y --assume-yes install libfreetype6-dev:$CROSS_ARCH \
libx11-dev:$CROSS_ARCH        \
libxinerama-dev:$CROSS_ARCH    \
libxrandr-dev:$CROSS_ARCH      \
libxcursor-dev:$CROSS_ARCH     \
mesa-common-dev:$CROSS_ARCH     \
libasound2-dev:$CROSS_ARCH      \
freeglut3-dev:$CROSS_ARCH      \
libxcomposite-dev:$CROSS_ARCH   \
libjack-dev:$CROSS_ARCH          \
libcurl4-openssl-dev:$CROSS_ARCH \
ladspa-sdk:$CROSS_ARCH

# libavahi-compat-libdnssd libfreetype6 libx11 libxinerama libxrandr libxcursor mesa-common libasound2 freeglut3 libxcomposite libjack libcurl4-openssl

apt-get -y -q --assume-yes install curl





SCRIPTPATH=`pwd`/$(dirname "$0") 
cd $SCRIPTPATH
cd ../../..

if [ ! -d "JUCE" ]; then
  JUCEV=`curl -s https://api.github.com/repos/juce-framework/JUCE/releases/latest | grep -oP '"tag_name": "\K(.*)(?=")'`
  JUCEURL="https://codeload.github.com/juce-framework/JUCE/tar.gz/$JUCEV"
  echo "downloading juce $JUCEV at $JUCEURL"
  wget  $JUCEURL -O JUCE.tar.gz
  tar -xf JUCE.tar.gz
  mv JUCE-master/ JUCE
  echo "downloading JUCE"
else
  echo "using system JUCE"
fi

apt-get -y -q --assume-yes install python3
apt-get -y -q --assume-yes install python3-pip

# rubberband
apt-get -y -q --assume-yes install librubberband-dev:$CROSS_ARCH
# for dns utility
# apt-get -y -q --assume-yes install libavahi-compat-libdnssd-dev:$CROSS_ARCH

#install add-apt-repository

apt-get  -y -q --assume-yes  install software-properties-common

# install cmake
# add-apt-repository ppa:george-edison55/cmake-3.x -y
# apt-get update
apt-get -y -q --assume-yes install cmake

cd LGML/third_party/libpdBuild/Linux
CFLAGS="$ARCH_FLAGS" LDFLAGS="$ARCH_FLAGS" cmake .. -DPD_MULTI:BOOL=ON  -DPD_UTILS:BOOL=OFF
cmake --build . -- VERBOSE=1

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


