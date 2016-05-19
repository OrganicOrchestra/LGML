/*
  ==============================================================================

    AudioOutNode.h
    Created: 7 Mar 2016 8:04:38pm
    Author:  Martin Hermant

  ==============================================================================
*/

#ifndef AUDIOOUTNODE_H_INCLUDED
#define AUDIOOUTNODE_H_INCLUDED



#include "NodeBase.h"

class AudioOutNode :
	public NodeBase,
	public juce::AudioProcessorGraph::AudioGraphIOProcessor,
	public ChangeListener
{
public:
	AudioOutNode(uint32 nodeId);;
	~AudioOutNode();


	void changeListenerCallback(ChangeBroadcaster* source)override;
	void updateIO();
	void processBlockInternal(AudioBuffer<float>& buffer, MidiBuffer& midiMessages) override;

    virtual NodeBaseUI * createUI() override;


    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(AudioOutNode)
};




#endif  // AUDIOOUTNODE_H_INCLUDED
