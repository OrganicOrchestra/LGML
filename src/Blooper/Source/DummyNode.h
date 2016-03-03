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
	};


	class DummyDataProcessor : public NodeBase::NodeDataProcessor
	{

		int getTotalNumInputChannels() { return 0; }
		int getTotalNumOutputChannels() { return 0; }


	};

	DummyNode() :NodeBase(new DummyAudioProcessor, new DummyDataProcessor) {};


	
};



#endif  // DUMMYNODE_H_INCLUDED
