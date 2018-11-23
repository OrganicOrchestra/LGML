/*
 ==============================================================================

 Copyright © Organic Orchestra, 2017

 This file is part of LGML. LGML is a software to manipulate sound in real-time

 This program is free software; you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation (version 3 of the License).

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

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
