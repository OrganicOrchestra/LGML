/* Copyright © Organic Orchestra, 2017
*
* This file is part of LGML.  LGML is a software to manipulate sound in realtime
*
* This program is free software; you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation (version 3 of the License).
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
*
*/


#ifndef AUDIOMIXERNODE_H_INCLUDED
#define AUDIOMIXERNODE_H_INCLUDED


#include "../NodeBase.h"


class AudioMixerNode : public NodeBase
{

public:
  DECLARE_OBJ_TYPE(AudioMixerNode);
	class OutputBus : public ParameterContainer {
	public:
		OutputBus(int _outputIndex, int numInput);
		void setNumInput(int numInput);

        void onContainerParameterChanged(Parameter *p)override;
		Array<FloatParameter*,CriticalSection> volumes;
        Array<float> logVolumes;
		Array<float> lastVolumes;
		int outputIndex;
	};


    
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

	



private:


    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(AudioMixerNode)
};

#endif  // AUDIOMIXERNODE_H_INCLUDED
