/*
  ==============================================================================

    JuceHeaderCore.h
    Created: 15 Sep 2017 7:06:59pm
    Author:  Martin Hermant

  ==============================================================================
*/

#pragma once
#ifndef JUCE_HEADERCORE_INCLUDED
#define JUCE_HEADERCORE_INCLUDED

//#include "JuceHeader.h"
//
#include "AppConfig.h"

#include <juce_core/juce_core.h>
#include <juce_cryptography/juce_cryptography.h>
#include <juce_data_structures/juce_data_structures.h>
#include <juce_events/juce_events.h>

#include "BinaryData.h"

#if ! DONT_SET_USING_JUCE_NAMESPACE
// If your code uses a lot of JUCE classes, then this will obviously save you
// a lot of typing, but can be disabled by setting DONT_SET_USING_JUCE_NAMESPACE.
using namespace juce;
#endif


#endif 
