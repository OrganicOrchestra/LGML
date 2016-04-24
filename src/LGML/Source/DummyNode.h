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
        DummyAudioProcessor():NodeBase::NodeAudioProcessor()
		{
			setPlayConfigDetails(2, 3, getSampleRate(), getBlockSize());

		}
        
		int step1 = 0;
		int step2 = 0;
		int period1 = (int)(44100 *1.0f/300);
		int period2 = (int)(44100 * 1.0f / 300);
		float amp = 1.f;

        void processBlockInternal(AudioBuffer<float>& buffer, MidiBuffer&) {

			//DBG("process block internal dummy");

			
            for(int i = 0 ; i < buffer.getNumSamples() ; i++){
                buffer.addSample(0, i, (float)(amp*cos(2.0*double_Pi*step1*1.0/period1)));
				buffer.addSample(1, i, (float)(amp*cos(2.0*double_Pi*step2*1.0 / period2)));
				step1++;
				step2++;
                if(step1>period1){step1 = 0;}
				if (step2>period2) { step2 = 0; }
            }


        }

        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(DummyAudioProcessor)
    };


    class DummyDataProcessor : public NodeBase::NodeDataProcessor
    {
    public:
        DummyDataProcessor() :NodeBase::NodeDataProcessor() {
		addInputData("IN Number", DataProcessor::DataType::Number);
		addInputData("IN Position", DataProcessor::DataType::Position);

		addOutputData("OUT Number", DataProcessor::DataType::Number);
		addOutputData("OUT Orientation", DataProcessor::DataType::Orientation);
		}

		
        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(DummyDataProcessor)
    };



    DummyNode(NodeManager * nodeManager,uint32 nodeId);
    ~DummyNode();

    //parameters
    FloatParameter * freq1Param;
	FloatParameter * freq2Param;

    Trigger * testTrigger;

    void onAnyParameterChanged(Parameter * p) override;

	void inputDataChanged(DataProcessor::Data *d) override
	{
		DBG("DummyNode :: Input data changed " << d->name);

		if (d->name == "IN Number")
		{
			((DummyAudioProcessor *)audioProcessor)->amp = d->getElement("value")->value;
		}
	}

    virtual NodeBaseUI * createUI() override;


    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(DummyNode)
};



#endif  // DUMMYNODE_H_INCLUDED
