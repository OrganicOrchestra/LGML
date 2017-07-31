/*
  ==============================================================================

    NetworkUtils.h
    Created: 30 Jul 2017 3:58:07pm
    Author:  Martin Hermant

  ==============================================================================
*/

#pragma once

#include "JuceHeader.h"

class Monitor;
class ZeroConfManager;
class ZeroConfListener;
class NetworkUtils{
public:
  juce_DeclareSingleton(NetworkUtils, true);

  NetworkUtils();
  ~NetworkUtils();
  static bool isValidIP(const String & ip);
  static IPAddress hostnameToIP(const String & hn);


};

