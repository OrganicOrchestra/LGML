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


#ifndef CONTAINEROUTNODE_H_INCLUDED
#define CONTAINEROUTNODE_H_INCLUDED


#include "../NodeBase.h"

class NodeContainer;

class ContainerOutNode : public NodeBase,public AudioProcessorGraph::AudioGraphIOProcessor
{
public:
  DECLARE_OBJ_TYPE(ContainerOutNode)
	~ContainerOutNode();


  void setParentNodeContainer(NodeContainer *) override;
  

	//AUDIO
	IntParameter * numChannels;
  void setNumChannels(int num);
  void processBlockInternal(AudioBuffer<float>& buffer, MidiBuffer & midiMessages)override;
  

	//DATA
	IntParameter * numInputData;

  void processInputDataChanged(Data * d) override;

	virtual ConnectableNodeUI * createUI() override;


	void onContainerParameterChanged(Parameter *) override;



	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ContainerOutNode)
};






#endif  // CONTAINEROUTNODE_H_INCLUDED
