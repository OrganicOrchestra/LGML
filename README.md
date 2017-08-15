[![Build Status](https://travis-ci.org/OrganicOrchestra/LGML.svg?branch=develop)](https://travis-ci.org/OrganicOrchestra/LGML)
[![CircleCI](https://circleci.com/gh/OrganicOrchestra/LGML.svg?style=svg)](https://circleci.com/gh/OrganicOrchestra/LGML)
[![Build status](https://ci.appveyor.com/api/projects/status/l1rd556j6ogglg6p?svg=true)](https://ci.appveyor.com/project/MartinHN/lgml)


LGML
(c) 2017 , LGML is a software to help live music performance

LGML rely on some awesome libraries :
 * JUCE : http://www.juce.com
 * Ableton Link : https://github.com/Ableton/link
 * serial lib : https://github.com/wjwwood/serial
 * rubberband : http://breakfastquay.com/rubberband/
 * VST3 SDK : www.steinberg.net
 * ASIO(Windows) : www.steinberg.net


BUILD
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

CONTENTS

* License
* Documentation Location
* Authors and contributors

LICENSE

read gpl-3.0.txt

DOCUMENTATION LOCATION
 
The documentation currently is located in the 'doc' directory.

--
This software is OSI Certified Open Source Software.
OSI Certified is a certification mark of the Open Source Initiative.
