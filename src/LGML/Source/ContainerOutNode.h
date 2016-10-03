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

class NodeContainer;
class ContainerInNode;

class ContainerOutNode : public NodeBase
{
public:
	ContainerOutNode();
	~ContainerOutNode();


  void setParentNodeContainer(NodeContainer *) override;
  

	//AUDIO
	IntParameter * numInputChannels;
	void setNumAudioChannels(int channels);
  void processBlockInternal(AudioBuffer<float>& buffer, MidiBuffer & midiMessages)override;


	//DATA
	IntParameter * numInputData;
	void processInputDataChanged(Data * d) override;

	virtual ConnectableNodeUI * createUI() override;


	void onContainerParameterChanged(Parameter *) override;

  ContainerInNode * linkedInNode;

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ContainerOutNode)
};






#endif  // CONTAINEROUTNODE_H_INCLUDED
