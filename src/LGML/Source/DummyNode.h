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
    DummyNode();
    ~DummyNode();

    //parameters
    FloatParameter * freq1Param;
    FloatParameter * freq2Param;

    Trigger * testTrigger;

    void onContainerParameterChanged(Parameter * p) override;

	//AUDIO

	int step1 = 0;
	int step2 = 0;
	int period1 = (int)(44100 * 1.0f / 300);
	int period2 = (int)(44100 * 1.0f / 300);
	float amp = 1.f;

	void processBlockInternal(AudioBuffer<float>& buffer, MidiBuffer&)override {

		//DBG("process block internal dummy");


		for (int i = 0; i < buffer.getNumSamples(); i++) {
			buffer.addSample(0, i, (float)(amp*cos(2.0*double_Pi*step1*1.0 / period1)));
			buffer.addSample(1, i, (float)(amp*cos(2.0*double_Pi*step2*1.0 / period2)));
			step1++;
			step2++;
			if (step1>period1) { step1 = 0; }
			if (step2>period2) { step2 = 0; }
		}


	}

	//DATA

	void processInputDataChanged(Data *d) override
	{
		DBG("DummyNode :: Input data changed " << d->name);

		if (d->name == "IN Number")
		{
			amp = d->getElement("value")->value;
		}
	}



    virtual ConnectableNodeUI * createUI() override;


    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(DummyNode)
};



#endif  // DUMMYNODE_H_INCLUDED
