#!/usr/bin/bash


set -x
if ! (command -v python3 &>/dev/null); then
    #install brew if needed
    if !(command -v brew &>/dev/null); then
      ruby -e "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/master/install)"
    fi

brew update >/dev/null
brew install python3
else
echo Python 3 is installed
fi


pip3 install -U pip; 
pip3 install -U dmgbuild;
pip3 install -U requests;
