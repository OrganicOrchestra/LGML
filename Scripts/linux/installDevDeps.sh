# this script install JUCE in sibling directory of LGML

if [ -z ${LGML_TARGET_ARCH+x} ]; then LGML_TARGET_ARCH=`dpkg --print-architecture`; else echo "arch is set to '$LGML_TARGET_ARCH'"; fi

# for dns utility
apt-get -y -q --force-yes install libavahi-compat-libdnssd-dev

## these are devloper libs needed for JUCE,   not sure wich are needed in released version...
# from Makefile alsa freetype2 libcurl x11 xext xinerama
apt-get -y -q --force-yes install libfreetype6-dev
apt-get -y -q --force-yes install libx11-dev
apt-get -y -q --force-yes install libxinerama-dev
apt-get -y -q --force-yes install libxrandr-dev
apt-get -y -q --force-yes install libxcursor-dev
apt-get -y -q --force-yes install mesa-common-dev
apt-get -y -q --force-yes install libasound2-dev
apt-get -y -q --force-yes install freeglut3-dev
apt-get -y -q --force-yes install libxcomposite-dev
apt-get -y -q --force-yes install libjack-dev
apt-get -y -q --force-yes install libcurl4-gnutls-dev

SCRIPTPATH=`pwd`/$(dirname "$0") 
cd $SCRIPTPATH
cd ../../..

pwd
ls


apt-get -y -q --force-yes install unzip
apt-get -y -q --force-yes install curl

if [ ! -d "JUCE" ]; then
  curl -L https://github.com/julianstorer/JUCE/archive/master.zip > JUCE.zip
  unzip -q JUCE.zip
  mv JUCE-master/ JUCE
fi


apt-get -y -q --force-yes install python

apt-get -y -q --force-yes install libcurl4-openssl-dev
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


