#!/usr/bin/bash

## these are devloper libs,   not sure wich are needed in released version...
xcode-select --install;
brew update;
brew install python;
which python;
pip install -U pip; 
pip install dmgbuild;