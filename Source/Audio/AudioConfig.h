/*
 ==============================================================================

 Copyright © Organic Orchestra, 2017

 This file is part of LGML. LGML is a software to manipulate sound in realtime

 This program is free software; you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation (version 3 of the License).

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

 ==============================================================================
 */

#ifndef AUDIOCONFIG_H_INCLUDED
#define AUDIOCONFIG_H_INCLUDED


#include "../JuceHeaderCore.h"


#if (defined JUCE_MAC || defined JUCE_LINUX || defined JUCE_WINDOWS || defined JUCE_IOS)
    #define BUFFER_CAN_STRETCH 1
#else
    #define BUFFER_CAN_STRETCH 0
#endif

#define DODBGAUDIO 0
#if DODBGAUDIO
#warning
#define DBGRT(x)  DBG(x)
#else
#define DBGRT(x)
#endif




#endif  // AUDIOCONFIG_H_INCLUDED
