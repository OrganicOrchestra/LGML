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
#include "EnumParameter.h"

class DummyNode : public NodeBase
{
public:
    DummyNode();
    ~DummyNode();

    //parameters
    FloatParameter * freq1Param;
    FloatParameter * freq2Param;
	EnumParameter * enumParam;

    Trigger * testTrigger;

    void onContainerParameterChanged(Parameter * p) override;

	//AUDIO

	int step1 = 0;
	int step2 = 0;
	int period1 = (int)(44100 * 1.0f / 300);
	int period2 = (int)(44100 * 1.0f / 300);
	float amp = 1.f;

	void processBlockInternal(AudioBuffer<float>& buffer, MidiBuffer&)override;

	//DATA
	void processInputDataChanged(Data *d) override;



    virtual ConnectableNodeUI * createUI() override;


    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(DummyNode)
};



#endif  // DUMMYNODE_H_INCLUDED
