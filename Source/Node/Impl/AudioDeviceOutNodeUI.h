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


#pragma once

#include "../UI/ConnectableNodeContentUI.h"
#include "../NodeBase.h"
#include "../../Controllable/Parameter/UI/ParameterUI.h"

template<typename T> class SliderUI;
class VuMeter;
class AudioDeviceOutNode;

class AudioDeviceOutNodeContentUI :
    public ConnectableNodeContentUI,
    public ConnectableNode::ConnectableNodeListener,
    public ChangeListener
{
public:
    AudioDeviceOutNodeContentUI();
    virtual ~AudioDeviceOutNodeContentUI();

    OwnedArray<ParameterUI> muteToggles;
    OwnedArray<SliderUI<floatParamType> >  volumes;
    OwnedArray<VuMeter> vuMeters;

    AudioDeviceOutNode* audioOutNode;

    void init() override;

    void resized() override;

    void updateVuMeters();

    void addVuMeter();
    void removeLastVuMeter();

    virtual void nodeParameterChangedAsync (ConnectableNode*, ParameterBase*) override;

    virtual void numAudioOutputChanged (ConnectableNode*, int newNum) override;
    virtual void numAudioInputChanged (ConnectableNode*, int newNum) override;

    void changeListenerCallback (ChangeBroadcaster* source)override;
    //virtual void numAudioOutputChanged() { DBG("Output changed !"); }

};


#if NON_INCREMENTAL_COMPILATION
    #include "AudioDeviceOutNodeUI.cpp"
#endif
