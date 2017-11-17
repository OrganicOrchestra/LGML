#!/usr/bin/bash



if command -v python3 &>/dev/null; then
    echo Python 3 is installed
else
    #install brew if needed
    if command -v brew &/dev/null; then
      ruby -e "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/master/install)"
    fi
    brew install python3
fi


pip3 install -U pip3; 
pip3 install dmgbuild;
pip3 install requests;
