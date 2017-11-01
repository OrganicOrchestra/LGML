# LGML
[![Build Status](https://travis-ci.org/OrganicOrchestra/LGML.svg?branch=develop)](https://travis-ci.org/OrganicOrchestra/LGML)
[![CircleCI](https://circleci.com/gh/OrganicOrchestra/LGML/tree/develop.svg?style=svg)](https://circleci.com/gh/OrganicOrchestra/LGML/tree/develop)
[![Build status](https://ci.appveyor.com/api/projects/status/bt4r87i23v7f44qq/branch/develop?svg=true)](https://ci.appveyor.com/project/OrganicOrchestra/lgml/branch/develop)


(c) Organic-Orchestra 2017 , 

LGML is a software to help live music performance , it's main features are :

* nodal audio routing
* audio plugin host (VST / AU)
* native plugin : looper / spatialisation
* easy interface with controller (OSC/MIDI/Serial)
* integrated script engine to go beyond the limits



## Contents

* Build instructions
* License
* Documentation
* Authors and contributors
* Third party software



## Build instructions

```
git clone https://github.com/OrganicOrchestra/LGML.git
git submodule update --init --recursive --remote
git clone https://github.com/WeAreROLI/JUCE.git
```
JUCE has to be in a sibling folder from LGML:
```
/path/of/LGML/
  LGML/
  JUCE/
```

see Scripts/yourplatform/ for platform specific setup :
* compiling rubberband



## License

read gpl-3.0.txt


## Documentation

Developer documentation : located in the 'doc' directory.
User documentation : work in progress... stay tuned


## Third party software

LGML rely on some awesome libraries :
 * JUCE : http://www.juce.com
 * Ableton Link : https://github.com/Ableton/link
 * serial lib : https://github.com/wjwwood/serial
 * rubberband : http://breakfastquay.com/rubberband/
 * VST3 SDK : www.steinberg.net
 * ASIO (Windows) : www.steinberg.net
 * Jack (Linux) : www.linuxmao.org/Jack


--
This software is OSI Certified Open Source Software.
OSI Certified is a certification mark of the Open Source Initiative.
