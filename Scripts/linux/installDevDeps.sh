# this script install JUCE in sibling directory of LGML


# for dns utility
sudo apt-get -y --force-yes install libavahi-compat-libdnssd-dev

## these are devloper libs needed for JUCE,   not sure wich are needed in released version...
sudo apt-get -y --force-yes install libfreetype6-dev
sudo apt-get -y --force-yes install libx11-dev
sudo apt-get -y --force-yes install libxinerama-dev
sudo apt-get -y --force-yes install libxrandr-dev
sudo apt-get -y --force-yes install libxcursor-dev
sudo apt-get -y --force-yes install mesa-common-dev
sudo apt-get -y --force-yes install libasound2-dev
sudo apt-get -y --force-yes install freeglut3-dev
sudo apt-get -y --force-yes install libxcomposite-dev
sudo apt-get -y --force-yes install libjack-dev


SCRIPTPATH=`pwd`/$(dirname "$0") 
cd $SCRIPTPATH
cd ../../..

pwd
ls

if [ ! -d "JUCE" ]; then
  curl -L https://github.com/julianstorer/JUCE/archive/master.zip > JUCE.zip
  unzip -q JUCE.zip
  mv JUCE-master/ JUCE
fi

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

