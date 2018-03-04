/* Copyright © Organic Orchestra, 2017
*
* This file is part of LGML.  LGML is a software to manipulate sound in realtime
*
* This program is free software; you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation (version 3 of the License).
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
*
*/


#ifndef DUMMYNODE_H_INCLUDED
#define DUMMYNODE_H_INCLUDED

#include "../NodeBase.h"
#if JUCE_DEBUG
#include "../../Controllable/Parameter/EnumParameter.h"

class DummyNode : public NodeBase
{
public:
    DECLARE_OBJ_TYPE (DummyNode)
    ~DummyNode();

    //parameters
    FloatParameter* freq1Param;
    FloatParameter* freq2Param;
    EnumParameter* enumParam;

    FloatParameter* pxParam;
    FloatParameter* pyParam;


    Trigger* testTrigger;


    void onContainerParameterChanged (Parameter* p) override;

    //AUDIO

    int step1 = 0;
    int step2 = 0;
    int period1 = (int) (44100 * 1.0f / 300);
    int period2 = (int) (44100 * 1.0f / 300);
    float amp = 1.f;

    void processBlockInternal (AudioBuffer<float>& buffer, MidiBuffer&)override;

    //DATA

    Data* outPosData;
    void processInputDataChanged (Data* d) override;





    FadeInOut clickFade;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (DummyNode)
};


#endif // JUCE DEBGUG

#endif  // DUMMYNODE_H_INCLUDED
