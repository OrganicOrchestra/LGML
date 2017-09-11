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


#ifndef AudioDeviceInNode_H_INCLUDED
#define AudioDeviceInNode_H_INCLUDED



#include "../NodeBase.h"


class AudioDeviceInNode :
	public NodeBase,
	public juce::AudioProcessorGraph::AudioGraphIOProcessor,
	public ChangeListener
{

public:

	AudioDeviceInNode();
	~AudioDeviceInNode();

	void processBlockInternal(AudioBuffer<float>& buffer, MidiBuffer& midiMessages)override;

	void changeListenerCallback(ChangeBroadcaster* source)override;
    void onContainerParameterChanged(Parameter *)override;
  void setParentNodeContainer(NodeContainer*)override;
	Array<BoolParameter *> inMutes;
    Array<FloatParameter * > volumes;
    Array<float > logVolumes,lastVolumes;
    IntParameter * desiredNumAudioInput;



	void addVolMute();
	void removeVolMute();
  void numChannelsChanged(bool isInput)override;
	virtual ConnectableNodeUI * createUI() override;



private:
    int lastNumberOfInputs;
    void    updateVolMutes();
	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(AudioDeviceInNode)
};







#endif  // AudioDeviceInNode_H_INCLUDED
