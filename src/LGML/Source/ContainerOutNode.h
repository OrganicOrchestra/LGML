/*
  ==============================================================================

    ContainerOutNode.h
    Created: 19 May 2016 5:44:22pm
    Author:  bkupe

  ==============================================================================
*/

#ifndef CONTAINEROUTNODE_H_INCLUDED
#define CONTAINEROUTNODE_H_INCLUDED


#include "NodeBase.h"


class ContainerOutNode : public NodeBase
{
public:
	ContainerOutNode();
	~ContainerOutNode();

	//AUDIO

	//DATA
	void processInputDataChanged(Data * d) override;

	virtual ConnectableNodeUI * createUI() override;


	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ContainerOutNode)
};






#endif  // CONTAINEROUTNODE_H_INCLUDED
