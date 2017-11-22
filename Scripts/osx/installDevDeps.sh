#!/usr/bin/bash
pwd;
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
	tar -xzf JUCE.zip;
	mv JUCE-master JUCE;
fi


# build rubberband
cd $SCRIPTPATH
xcodebuild -find clang++
cd ../../third_party/rubberband
# ./configure
mkdir -p lib
CXX=gcc -stdlib=libc++ make -f Makefile.osx CPPFLAGS=-DNO_THREADING static