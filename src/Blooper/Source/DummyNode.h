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


class DummyNode : public NodeBase 
{
public:
	class DummyAudioProcessor : public NodeBase::NodeAudioProcessor
	{
	public:
		DummyAudioProcessor():NodeBase::NodeAudioProcessor(){}

		void processBlock(AudioBuffer<float>& buffer,
			MidiBuffer& midiMessages) {
            int dbg=0;
        }

		JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(DummyAudioProcessor)
	};


	class DummyDataProcessor : public NodeBase::NodeDataProcessor 
	{
	public:
		DummyDataProcessor() :NodeBase::NodeDataProcessor() {}

		virtual void processData(Data * incomingData, const String &targetInputDataName, const String &targetDataComponentName) {}

		JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(DummyDataProcessor)
	};



	DummyNode(NodeManager * nodeManager,uint32 nodeId);
	~DummyNode();

	virtual NodeBaseUI * createUI() override;
	

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(DummyNode)
};



#endif  // DUMMYNODE_H_INCLUDED
