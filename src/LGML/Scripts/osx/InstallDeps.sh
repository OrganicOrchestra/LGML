#!/usr/bin/bash

mkdir "~/SDKs/VST3 SDK";
curl -L http://www.steinberg.net/sdk_downloads/vstsdk366_27_06_2016_build_61.zip > VST3.zip
tar -xzf VST3.zip;
mv "VST3 SDK/" "~/SDKs/VST3 SDK";

curl -L https://github.com/julianstorer/JUCE/archive/master.zip > JUCE.zip;
tar -xzf JUCE.zip;
mv JUCE-master/ JUCE;


# git clone "https://github.com/julianstorer/JUCE.git" --depth 1;
# pwd && ls;