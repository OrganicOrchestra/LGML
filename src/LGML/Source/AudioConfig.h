/*
  ==============================================================================

    AudioConfig.h
    Created: 16 Feb 2017 8:11:16pm
    Author:  Martin Hermant

  ==============================================================================
*/

#ifndef AUDIOCONFIG_H_INCLUDED
#define AUDIOCONFIG_H_INCLUDED

#include "JuceHeader.h"

#ifndef JUCE_MAC
#error whatwhat?
#endif
// TODO change when windows / linux support
#ifdef JUCE_MAC
#define BUFFER_CAN_STRETCH 1
#else
#define BUFFER_CAN_STRETCH 0
#endif



#endif  // AUDIOCONFIG_H_INCLUDED
