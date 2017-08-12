# this script install JUCE in sibling directory of LGML


# for dns utility
apt-get -y --force-yes install libavahi-compat-libdnssd-dev

## these are devloper libs needed for JUCE,   not sure wich are needed in released version...
apt-get -y --force-yes install libfreetype6-dev
apt-get -y --force-yes install libx11-dev
apt-get -y --force-yes install libxinerama-dev
apt-get -y --force-yes install libxrandr-dev
apt-get -y --force-yes install libxcursor-dev
apt-get -y --force-yes install mesa-common-dev
apt-get -y --force-yes install libasound2-dev
apt-get -y --force-yes install freeglut3-dev
apt-get -y --force-yes install libxcomposite-dev
apt-get -y --force-yes install libjack-dev


SCRIPTPATH=`pwd`/$(dirname "$0") 
cd $SCRIPTPATH
cd ../../..

pwd
ls

apt-get -y --force-yes install curl
apt-get -y --force-yes install unzip

if [ ! -d "JUCE" ]; then
  curl -L https://github.com/julianstorer/JUCE/archive/master.zip > JUCE.zip
  unzip -q JUCE.zip
  mv JUCE-master/ JUCE
fi


apt-get -y --force-yes install python

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


