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

class ContainerInNode : public NodeBase
{
public:
	ContainerInNode();
	~ContainerInNode();

	//AUDIO
	void setNumAudioChannels(int channels);

	//DATA
	void processInputDataChanged(Data * d) override;

	virtual ConnectableNodeUI * createUI() override;


	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ContainerInNode)
};



#endif  // CONTAINERINNODE_H_INCLUDED
