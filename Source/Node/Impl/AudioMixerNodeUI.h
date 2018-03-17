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


#ifndef AUDIOMIXERNODEUI_H_INCLUDED
#define AUDIOMIXERNODEUI_H_INCLUDED


#include "AudioMixerNode.h"
#include "../UI/ConnectableNodeContentUI.h"


#include "../../Controllable/Parameter/UI/SliderUI.h"

class AudioMixerNodeUI : public ConnectableNodeContentUI,
    public ConnectableNode::ConnectableNodeListener,
    private ParameterBase::Listener
{
public:
    AudioMixerNodeUI()
    {
    }

    ~AudioMixerNodeUI();
    void init() override;

    void numAudioInputChanged (ConnectableNode*, int )override;
    void numAudioOutputChanged (ConnectableNode*, int )override;

    void numAudioInputChangedUI (ConnectableNode* c, int numInput);

    void        numAudioOutputChangedUI (ConnectableNode* c, int numInput);



    class OutputBusUI : public juce::Component
    {
    public:

        OwnedArray<FloatSliderUI> inputVolumes;


        OutputBusUI (AudioMixerNode::OutputBus* o): owner (o)
        {

            setNumInput (o->volumes.size());
        };

        ~OutputBusUI()
        {

        }
        void setNumInput (int numInput);
        void resized() override;
        int outputIdx;
        AudioMixerNode::OutputBus* owner;
        BigInteger visibleChanels;
        void setOneVisible (int num);
        int getNumOfVisibleChannels();
        void updateVisibleChannels();
        void setAllVisible();
        void handleCommandMessage (int id)override;

    };

    void handleCommandMessage (int id)override;
    void parameterValueChanged ( ParameterBase* p, ParameterBase::Listener * notifier=nullptr) override;
    void resized() override;
    OwnedArray<OutputBusUI> outputBusUIs;
    AudioMixerNode* mixerNode;

    void setOneToOne (bool);

};



#endif  // AUDIOMIXERNODEUI_H_INCLUDED
