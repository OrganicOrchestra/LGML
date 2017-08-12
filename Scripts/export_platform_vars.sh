#!/usr/bin/bash

platform=`uname -a | cut -f 1 -d  " "`

if [ "$platform" = "Darwin" ]; then
  export LGML_PLATFORM_TYPE='osx'
fi
if [ "$platform" = "Linux" ]; then
  export LGML_PLATFORM_TYPE='linux'
fi
if [ -z "$LGML_PLATFORM_TYPE" ]; then
  echo $platform not found from `uname -a`
  exit 1
fi

echo found platform $LGML_PLATFORM_TYPE