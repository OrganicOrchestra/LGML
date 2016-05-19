/*
  ==============================================================================

    AudioInNode.h
    Created: 7 Mar 2016 8:03:48pm
    Author:  Martin Hermant

  ==============================================================================
*/

#ifndef AUDIOINNODE_H_INCLUDED
#define AUDIOINNODE_H_INCLUDED



#include "NodeBase.h"


class AudioInNode :
	public NodeBase,
	public juce::AudioProcessorGraph::AudioGraphIOProcessor,
	public ChangeListener
{

public:

	AudioInNode(uint32 nodeId);;
	~AudioInNode();


	void processBlockInternal(AudioBuffer<float>& buffer, MidiBuffer& midiMessages)override;

	void changeListenerCallback(ChangeBroadcaster* source)override;
	void updateIO();


    virtual NodeBaseUI * createUI() override;


    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(AudioInNode)
};







#endif  // AUDIOINNODE_H_INCLUDED
