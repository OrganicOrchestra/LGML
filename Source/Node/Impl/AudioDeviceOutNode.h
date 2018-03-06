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


#ifndef AudioDeviceOutNode_H_INCLUDED
#define AudioDeviceOutNode_H_INCLUDED



#include "../NodeBase.h"

class AudioDeviceOutNode :
    public NodeBase,
    public juce::AudioProcessorGraph::AudioGraphIOProcessor,
    public ChangeListener

{
public:
    DECLARE_OBJ_TYPE (AudioDeviceOutNode,"access your sound card output");
    ~AudioDeviceOutNode();


    void changeListenerCallback (ChangeBroadcaster* source)override;
    void processBlockInternal (AudioBuffer<float>& buffer, MidiBuffer& midiMessages) override;

    Array<BoolParameter*> outMutes;
    Array<FloatParameter* > volumes;
    Array<float > logVolumes, lastVolumes;
    IntParameter* desiredNumAudioOutput;

    void addVolMute();
    void removeVolMute();

    void numChannelsChanged (bool isInput)override;

    void onContainerParameterChanged (Parameter* p) override;
private :
    void updateVolMutes();
    int lastNumberOfOutputs;

    void  setParentNodeContainer (NodeContainer* parent)override;

    Array<Array<float>> audioInCache;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (AudioDeviceOutNode)
};




#endif  // AudioDeviceOutNode_H_INCLUDED
