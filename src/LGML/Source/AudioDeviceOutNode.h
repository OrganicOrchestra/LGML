/*
==============================================================================

AudioDeviceOutNode.h
Created: 7 Mar 2016 8:04:38pm
Author:  Martin Hermant

==============================================================================
*/

#ifndef AudioDeviceOutNode_H_INCLUDED
#define AudioDeviceOutNode_H_INCLUDED



#include "NodeBase.h"

class AudioDeviceOutNode :
	public NodeBase,
	public juce::AudioProcessorGraph::AudioGraphIOProcessor,
	public ChangeListener
{
public:
	AudioDeviceOutNode();
	~AudioDeviceOutNode();


	void changeListenerCallback(ChangeBroadcaster* source)override;
	void updateIO();
	void processBlockInternal(AudioBuffer<float>& buffer, MidiBuffer& midiMessages) override;

	virtual NodeBaseUI * createUI() override;


	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(AudioDeviceOutNode)
};




#endif  // AudioDeviceOutNode_H_INCLUDED
