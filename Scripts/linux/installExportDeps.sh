#!/usr/bin/bash

# violent way to install right dependencyes (for python 2 or 3)
set +e
pip install -U pip; 
pip install future requests;
pip3 install -U pip3; 
pip3 install requests;

set -e