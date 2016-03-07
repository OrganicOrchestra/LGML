/*
  ==============================================================================

    AudioMixerNode.h
    Created: 3 Mar 2016 10:14:46am
    Author:  bkupe

  ==============================================================================
*/

#ifndef AUDIOMIXERNODE_H_INCLUDED
#define AUDIOMIXERNODE_H_INCLUDED


#include "NodeBase.h"
class NodeBaseUI;

class AudioMixerNode : public NodeBase
{

public:

	class AudioMixerAudioProcessor : public NodeBase::NodeAudioProcessor
	{
	public:
		AudioMixerAudioProcessor() :NodeBase::NodeAudioProcessor() {}

		void processBlockInternal(AudioBuffer<float>& buffer,
			MidiBuffer& midiMessages) {}

		JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(AudioMixerAudioProcessor)
	};

	AudioMixerNode(NodeManager * nodeManager,uint32 nodeId) :NodeBase(nodeManager,nodeId, "AudioMixer", new AudioMixerAudioProcessor, nullptr) {}


	virtual NodeBaseUI * createUI() override;

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(AudioMixerNode)
};

#endif  // AUDIOMIXERNODE_H_INCLUDED
