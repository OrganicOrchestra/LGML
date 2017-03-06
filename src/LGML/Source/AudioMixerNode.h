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


class AudioMixerNode : public NodeBase
{

public:
	class OutputBus : public ControllableContainer {
	public:
		OutputBus(int _outputIndex, int numInput);
		void setNumInput(int numInput);

        void onContainerParameterChanged(Parameter *p)override;
		Array<FloatParameter*,CriticalSection> volumes;
        Array<float> logVolumes;
		Array<float> lastVolumes;
		int outputIndex;
	};


    AudioMixerNode() ;
  void setParentNodeContainer(NodeContainer * c)override;

	OwnedArray<OutputBus> outBuses;
//  OwnedArray<Vumeter>
	AudioBuffer<float> cachedBuffer;


	IntParameter * numberOfInput;
	IntParameter * numberOfOutput;
    BoolParameter * oneToOne;

	void updateInput();
	void updateOutput();
  void numChannelsChanged(bool isInput) override;


	void processBlockInternal(AudioBuffer<float>& buffer, MidiBuffer&)override;
	void onContainerParameterChanged(Parameter * p) override;

	ConnectableNodeUI * createUI() override;



private:


    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(AudioMixerNode)
};

#endif  // AUDIOMIXERNODE_H_INCLUDED
