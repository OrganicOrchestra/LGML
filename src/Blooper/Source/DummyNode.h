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
        int step = 0;
        int period = 44100*1.0/440;
        float amp = 1;
		void processBlockInternal(AudioBuffer<float>& buffer,
			MidiBuffer& midiMessages) {
            
            for(int i = 0 ; i < buffer.getNumSamples() ; i++){
                buffer.addSample(0, i, amp*cos(2.0*double_Pi*step*1.0/period));
                step++;
                if(step>period){
                    step = 0;
                }
            }

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
