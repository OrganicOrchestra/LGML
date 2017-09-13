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


#ifndef CONTAINERINNODE_H_INCLUDED
#define CONTAINERINNODE_H_INCLUDED



#include "../NodeBase.h"
class NodeContainer;

class ContainerInNode :
	public NodeBase,
virtual public AudioProcessorGraph::AudioGraphIOProcessor
{
public:
  DECLARE_OBJ_TYPE(ContainerInNode)
	~ContainerInNode();


	void setParentNodeContainer(NodeContainer * nc)override;


	//AUDIO
	IntParameter * numChannels;
  void setNumChannels(int num);
  void processBlockInternal(AudioBuffer<float>& buffer, MidiBuffer & midiMessages) override;


	//DATA
	IntParameter * numInputData;
	void processInputDataChanged(Data * d) override;

	virtual ConnectableNodeUI * createUI() override;

	void onContainerParameterChanged(Parameter *) override;



	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ContainerInNode)
};



#endif  // CONTAINERINNODE_H_INCLUDED
