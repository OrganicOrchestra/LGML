/*
  ==============================================================================

    ContainerInNode.h
    Created: 19 May 2016 5:44:32pm
    Author:  bkupe

  ==============================================================================
*/

#ifndef CONTAINERINNODE_H_INCLUDED
#define CONTAINERINNODE_H_INCLUDED



#include "NodeBase.h"
class NodeContainer;

class ContainerInNode :
	public NodeBase
{
public:
	ContainerInNode();
	~ContainerInNode();

	void setParentNodeContainer(NodeContainer * nc)override;

	//AUDIO
	IntParameter * numInputChannels;
	void setNumAudioChannels(int channels);

	//DATA
	IntParameter * numInputData;
	void processInputDataChanged(Data * d) override;

	virtual ConnectableNodeUI * createUI() override;

	void onContainerParameterChanged(Parameter *) override;


	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ContainerInNode)
};



#endif  // CONTAINERINNODE_H_INCLUDED
