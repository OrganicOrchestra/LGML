#!/usr/bin/bash

# this script install :
#       JUCE in sibling directory of LGML
#       compile rubberband locally (third_party/rubberband)


SCRIPTPATH=`pwd`/$(dirname "$0") 
echo $SCRIPTPATH
cd $SCRIPTPATH
cd ../../..


# install command line tools if needed
xcode-select --install

# install JUCE if needed
if [ ! -d "JUCE" ]; then
	curl -L https://github.com/julianstorer/JUCE/archive/master.zip > JUCE.zip;
	unzip -oq JUCE.zip;
	mv JUCE-master JUCE;
fi


# build rubberband
cd $SCRIPTPATH/../../third_party/rubberband
echo "original CXX : $CXX"
CXX=`xcodebuild -find clang++`
CXX="$CXX -stdlib=libc++"
ARCHFLAGS="-arch i386 -mmacosx-version-min=10.7"
echo $CXX
# ./configure
mkdir -p lib
make -f Makefile.osx CPPFLAGS=-DNO_THREADING static -j4

# build rubberband
cd $SCRIPTPATH/../../third_party/libpd
xcodebuild -project libpd.xcodeproj -scheme libpd-osx-multi -configuration Release

