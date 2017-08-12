#!/usr/bin/env bash

BASEDIR=$(dirname "$0") 
cd $BASEDIR
cd ../../third_party/rubberband
# ./configure
mkdir -p lib
make -f Makefile.osx CPPFLAGS=-DNO_THREADING static
