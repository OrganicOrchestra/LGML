#!/usr/bin/bash

## these are devloper libs,   not sure wich are needed in released version...
brew update;
brew install python;
which python;
pip install -U pip; 
pip install dmgbuild;
pwd;
git clone https://github.com/julianstorer/JUCE.git;