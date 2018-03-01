# this script install deps needed to compile LGML from source :
# ** /!\ /!\ /!\ /!\ gcc-5 will be installed for c++14 support /!\ /!\ /!\ /!\  (on ubuntu <16 for example)
# ** JUCE is downloaded in a sibling directory of LGML
# ** JUCE deps also
# this script support cross-compiling setting the CROSS_ARCH flag to armhf (see circleci config.yml)

set +e # ignore apt update error
NATIVE_ARCH=`dpkg --print-architecture`
if [ -z ${CROSS_ARCH+x} ]; then CROSS_ARCH="$NATIVE_ARCH"; fi
if [ "$CROSS_ARCH" != "$NATIVE_ARCH" ]; then echo "adding foreing arch $CROSS_ARCH"; dpkg --add-architecture $CROSS_ARCH;apt-get -qq update; fi
echo "arch is set to '$CROSS_ARCH'"

set -e # un-ignore apt update error

## these are devloper libs needed for JUCE,   not sure wich are needed in released version...
# from Makefile alsa freetype2 libcurl x11 xext xinerama
apt-get -y --assume-yes install libfreetype6-dev:$CROSS_ARCH 
apt-get -y --assume-yes install libx11-dev:$CROSS_ARCH
apt-get -y --assume-yes install libxinerama-dev:$CROSS_ARCH
apt-get -y --assume-yes install libxrandr-dev:$CROSS_ARCH
apt-get -y --assume-yes install libxcursor-dev:$CROSS_ARCH
apt-get -y --assume-yes install mesa-common-dev:$CROSS_ARCH
apt-get -y --assume-yes install libasound2-dev:$CROSS_ARCH
apt-get -y --assume-yes install freeglut3-dev:$CROSS_ARCH
apt-get -y --assume-yes install libxcomposite-dev:$CROSS_ARCH
apt-get -y --assume-yes install libjack-dev:$CROSS_ARCH
apt-get -y -q --assume-yes install libcurl4-openssl-dev:$CROSS_ARCH
apt-get -y -q --assume-yes install ladspa-sdk:$CROSS_ARCH

# libavahi-compat-libdnssd libfreetype6 libx11 libxinerama libxrandr libxcursor mesa-common libasound2 freeglut3 libxcomposite libjack libcurl4-openssl

apt-get -y -q --assume-yes install curl

echo "checking if gcc5"
GCCVERSIONGTEQ5=$(expr `gcc -dumpversion | cut -f1 -d.` \>= 5)

echo $GCCVERSIONGTEQ5

# we are using c++14 features now
if [ "$GCCVERSIONGTEQ5" -eq "0" ] ; then
  echo "gcc version is too low, 5 minimum for c++14 support, trying to install one now"
  add-apt-repository ppa:ubuntu-toolchain-r/test
  apt-get update
  apt-get install gcc-5 g++-5
  update-alternatives --install /usr/bin/gcc gcc /usr/bin/gcc-5 60 --slave /usr/bin/g++ g++ /usr/bin/g++-5
fi



SCRIPTPATH=`pwd`/$(dirname "$0") 
cd $SCRIPTPATH
cd ../../..

if [ ! -d "JUCE" ]; then
  curl -L https://github.com/julianstorer/JUCE/archive/master.tar.gz > JUCE.tar.gz
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
apt-get -y -q --assume-yes install libavahi-compat-libdnssd-dev:$CROSS_ARCH


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


