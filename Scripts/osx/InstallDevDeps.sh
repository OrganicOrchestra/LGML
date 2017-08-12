#!/usr/bin/bash
pwd;


BASEDIR=$(dirname "$0") 
cd $BASEDIR
cd ../../..

# install JUCE if needed
if [ ! -d "JUCE" ]; then
	curl -L https://github.com/julianstorer/JUCE/archive/master.zip > JUCE.zip;
	tar -xzf JUCE.zip;
	mv JUCE-master JUCE;
fi

