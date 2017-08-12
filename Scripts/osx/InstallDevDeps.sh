#!/usr/bin/bash
pwd;

# go to home
cd ~;


# install JUCE if needed
if [ ! -d "Dev/JUCE" ]; then
	mkdir -p "Dev";
	curl -L https://github.com/julianstorer/JUCE/archive/master.zip > JUCE.zip;
	tar -xzf JUCE.zip;
	mv JUCE-master Dev/JUCE;
fi

