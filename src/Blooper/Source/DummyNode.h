/*
  ==============================================================================

    DummyNode.h
    Created: 3 Mar 2016 12:31:33pm
    Author:  bkupe

  ==============================================================================
*/

#ifndef DUMMYNODE_H_INCLUDED
#define DUMMYNODE_H_INCLUDED

#include "NodeBase.h"

class DummyNode : NodeBase
{
public:
	class DummyAudioProcessor : public NodeBase::NodeAudioProcessor
	{
	public:
		DummyAudioProcessor():NodeBase::NodeAudioProcessor(){}

		void processBlock(AudioBuffer<float>& buffer,
			MidiBuffer& midiMessages) {}

		JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(DummyAudioProcessor)
	};


	class DummyDataProcessor : public NodeBase::NodeDataProcessor
	{
	public:
		DummyDataProcessor() :NodeBase::NodeDataProcessor() {}

		virtual void processData(Data * incomingData,
			String targetInputDataName,
			String targetDataComponentName) {}

		JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(DummyDataProcessor)
	};

	DummyNode(uint32 nodeId) :NodeBase(nodeId, new DummyAudioProcessor, new DummyDataProcessor) {};


	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(DummyNode)
};



#endif  // DUMMYNODE_H_INCLUDED
